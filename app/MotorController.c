#include "motorController.h"

#define ALIGNMENT_DUTY_CYCLE            400
#define RAMP_DUTY_CYCLE                 400
#define ALIGNMENT_PERIOD                2000
#define PHASE_TRAVEL_TIMEOUT            4000
#define RAMP_ERPM_MAX                   4000
#define RAMP_HOLD_PERIOD                4000
#define RAMP_RATE                       4000
#define RAMP_END_PHASE_TRAVEL_PERIOD    200
#define STABILIZATION_PERIOD            100
#define COMMUTATION_BLANKING_PERIOD     1

static enum {
    STATE_IDLE,
    STATE_ALIGN,
    STATE_RAMP,
    STATE_CONSTANT_RPM_HOLD,
    STATE_CONSTANT_WAIT_AB_PHASE,
    STATE_COMMUTATION_BLANKING,
    STATE_ZERO_CROSS_DETECTION,
    STATE_SYNCHRO_COMMUTATION,
} motorControllerState;

static Phase phase;

static bool isZeroCrossDetected;
static bool isBlindCommutation;
static bool isMotorRunning;
static bool isRisingEdge;
static bool isSynchroCommutation;

static uint32_t bemfRisingPercentage;
static uint32_t bemfFallingPercentage;
static uint32_t phaseTravelTime;

static uint32_t currentPhazeTravelPeriod;
static uint32_t rampSpeed;
static uint32_t rampCounter;

static uint32_t alignStepCounter;
static uint32_t stabilizationDutyCycleCounter;
static uint32_t currentDutyCycle;

static uint32_t phaseTravelCounter;
static uint32_t commutationBlankingCounter;
static uint32_t comutationPeriod;

static uint32_t constantRpmCounter;
static uint32_t alignmentPeriodCounter;


static inline uint32_t motorControllerIirFilt(uint32_t value, uint32_t newValue)
{
    #define A   56
    #define B   ((1 << 8) - A)

    return (newValue * B + value * A) >> 8;
}

static void motorControllerReset(void)
{
    motorControllerState = STATE_IDLE;

    isSynchroCommutation = false;
    isZeroCrossDetected = false;
    isBlindCommutation = false;
    isRisingEdge = false;

    phase = PHASE_AB;

    phaseTravelCounter = 0;
    commutationBlankingCounter = 0;
    comutationPeriod = 0;
    constantRpmCounter = 0;
    alignmentPeriodCounter = 0;

    currentPhazeTravelPeriod = 0;
    currentDutyCycle = RAMP_DUTY_CYCLE;
    stabilizationDutyCycleCounter = STABILIZATION_PERIOD;
    phaseTravelTime = 4000;
    alignStepCounter = 0;
    rampSpeed = 0;
    rampCounter = 0;
    bemfFallingPercentage = 128;
    bemfRisingPercentage = 128;
}

void motorControllerStartMotor(bool isStart)
{
    isMotorRunning = isStart;
}

void motorControllerInit(void)
{
    motorControllerReset();
}

Phase motorControllerGetPhase(void)
{
    return phase;
}

void motorControllerSetDutyCycle(uint32_t dutyCycle)
{
    if (stabilizationDutyCycleCounter == 0) {
        currentDutyCycle = dutyCycle;
    }
}

void motorControllerProcess(uint16_t bEmf, uint16_t dcBus)
{
    if (isMotorRunning == false) {
        motorControllerState = STATE_IDLE;
        halPwmDisable();
        return;
    }

    currentPhazeTravelPeriod++;

    switch (motorControllerState) {
    case STATE_IDLE:
        halPwmDisable();
        motorControllerReset();
        if (isMotorRunning) {
            phase = PHASE_AB;
            halPwmSetDutyCycle(ALIGNMENT_DUTY_CYCLE);
            halPwmSetPhase(phase);
            alignStepCounter = 0;
            motorControllerState = STATE_ALIGN;
        }
        break;

    case STATE_ALIGN:
        if (alignStepCounter++ >= ALIGNMENT_PERIOD) {
            rampSpeed = 0;
            phaseTravelCounter = 0;
            isSynchroCommutation = false;
            isBlindCommutation = true;
            halPwmSetDutyCycle(RAMP_DUTY_CYCLE);
            motorControllerState = STATE_RAMP;
        }
        break;

    case STATE_RAMP:
        rampSpeed += RAMP_RATE;
        rampCounter = UINT32_MAX / rampSpeed;
        if (rampCounter > 30000) {
            rampCounter = 30000;
        }
        phaseTravelTime = rampCounter;

        if (phaseTravelTime <= RAMP_END_PHASE_TRAVEL_PERIOD) {
            constantRpmCounter = 0;
            motorControllerState = STATE_CONSTANT_RPM_HOLD;
        }
        break;

    case STATE_CONSTANT_RPM_HOLD:
        if (constantRpmCounter++ >= RAMP_HOLD_PERIOD) {
            motorControllerState = STATE_CONSTANT_WAIT_AB_PHASE;
        }
        break;

    case STATE_CONSTANT_WAIT_AB_PHASE:
        if (phase == PHASE_AB) {
            commutationBlankingCounter = 0;
            motorControllerState = STATE_COMMUTATION_BLANKING;
        }
        break;

    case STATE_COMMUTATION_BLANKING:
        if (commutationBlankingCounter++ >= COMMUTATION_BLANKING_PERIOD) {
            motorControllerState = STATE_ZERO_CROSS_DETECTION;
        } else {
            break;
        }

    case STATE_ZERO_CROSS_DETECTION:
        if (isZeroCrossDetected && (currentPhazeTravelPeriod > PHASE_TRAVEL_TIMEOUT)) {
            motorControllerState = STATE_IDLE;
            break;
        }

        if ((isRisingEdge && ((bEmf > dcBus) || (currentPhazeTravelPeriod > PHASE_TRAVEL_TIMEOUT))) ||
            (!isRisingEdge && ((bEmf < dcBus) || (currentPhazeTravelPeriod > PHASE_TRAVEL_TIMEOUT)))) {
                if (isZeroCrossDetected) {
                    phaseTravelTime = motorControllerIirFilt(phaseTravelTime, currentPhazeTravelPeriod);
                }
                comutationPeriod = (phaseTravelTime * bemfRisingPercentage) >> 8;
                currentPhazeTravelPeriod = 0;
                phaseTravelCounter = 0;
                isRisingEdge = !isRisingEdge;
                isZeroCrossDetected = true;
                isSynchroCommutation = true;
                isBlindCommutation = false;
                motorControllerState = STATE_SYNCHRO_COMMUTATION;
        }
        break;

    case STATE_SYNCHRO_COMMUTATION:

        break;
    }

    if (isSynchroCommutation) {
        halPwmSetDutyCycle(currentDutyCycle);
        if (phaseTravelCounter++ >= comutationPeriod) {
            if (++phase > PHASE_CB) {
                phase = PHASE_AB;
            }
            motorControllerState = STATE_COMMUTATION_BLANKING;
            isSynchroCommutation = false;
            halPwmSetPhase(phase);
            commutationBlankingCounter = 0;
            if (stabilizationDutyCycleCounter != 0) {
                 stabilizationDutyCycleCounter--;
            }
        }
    }

    if (isBlindCommutation) {
        if (phaseTravelCounter++ > phaseTravelTime) {
            phaseTravelCounter = 0;
            if (++phase > PHASE_CB) {
                phase = PHASE_AB;
            }
            halPwmSetPhase(phase);
        }
    }
}
