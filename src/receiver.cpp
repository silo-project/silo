#include "receiver.h"



Receiver::~Receiver() {
    for (auto input : this->inputs) {
        delete input;
    }

    for (auto previous : this->previousInput.size()) {
        delete previous;
    }
}


bool Receiver::getBit(uint8_t digit, uint64_t value) const {
    return (value >> digit) & 1;
}

std::vector< Value* > Receiver::getInputs() const {
    return this->inputs;
}

std::vector< Value* > Receiver::getPreviousInput() const {
    return this->previousInput;
}

std::vector< uint8_t > Receiver::getInputWidths() const {
    return this->inputWidths;
}



void Receiver::addInput(Value* value) {
    this->inputs.push_back(value);
    this->previousInput.push_back(value);
}

SingleBit Receiver::addSingleSignal(SingleBit nowValue, SingleBit newValue) {
    SingleBit result;

    result.error = nowValue.error || newValue.error;

    if (result.error) {
        return result;
    }

    if (nowValue.unknown || (!nowValue.unknown && (nowValue.value == newValue.value))) {
        result.value = newValue.value;
        result.unknown = nowValue.unknown || newValue.unknown;

        return result;
    }

    errorFlag = INPUT_VALUE_CONFLICT_WARNING;
    result.error = 1;

    return result;
}

Value* Receiver::addSignal(Value* nowSignal, Value* newSignal, uint8_t width) {
    SingleBit nowSignalBit;
    SingleBit newSignalBit;

    uint64_t nowValue = nowSignal->getValue();
    uint64_t newValue = newSignal->getValue();
    uint64_t nowUnknown = nowSignal->getUnknown();

    uint64_t newUnknown = newSignal->getUnknown();
    uint64_t nowError = nowSignal->getError();
    uint64_t newError = newSignal->getError();

    delete nowSignal;
    delete newSignal;

    uint64_t value = 0;
    uint64_t unknown = 0;
    uint64_t error = 0;

    for (int i = 0; i < width; ++i) {
        nowSignalBit.value = this->getBit(i, nowValue);
        nowSignalBit.unknown = this->getBit(i, nowUnknown);
        nowSignalBit.error = this->getBit(i, nowError);

        newSignalBit.value = this->getBit(i, newValue);
        newSignalBit.unknown = this->getBit(i, newUnknown);
        newSignalBit.error = this->getBit(i, newError);

        SingleBit signalBit = this->addSingleSignal(nowSignalBit, newSignalBit);

        value = this->setBit(i, value, signalBit.value);
        unknown = this->setBit(i, unknown, signalBit.unknown);
        error = this->setBit(i, error, signalBit.error);
    }

    return new Value(width, value, unknown, error);
}



uint64_t Receiver::setBit(uint8_t digit, uint64_t value, bool setValue) const {
    if (setValue) {
        return value | (1 << digit);
    } 
    else {
        return value & ~(1 << digit);
    }
}

SILO_STATUS Receiver::setInput(uint8_t inputNumber, Value *inputValue) {
    if (firstInputFlag) {
        this->firstInputFlag = false;
        delete this->inputs[inputNumber];
        this->inputs[inputNumber] = inputValue;
    } 
    else {
        this->inputs[inputNumber] = this->addSignal(inputs[inputNumber], inputValue, inputValue->getWidth());
    }

    SILO_STATUS warning = errorFlag;
    errorFlag = 0;

    return warning;
}

void Receiver::setInputs(const std::vector< Value* >& inputValues) {
    for (auto input : this->inputs) {
        delete input;
    }

    this->inputs = inputValues;
}

void Receiver::setPreviousInput(uint8_t inputNumber, Value inputValue) {
    delete this->previousInput[inputNumber];
    this->previousInput[inputNumber] = &inputValue;
}

void Receiver::setPreviousInputs(const std::vector< Value* >& inputValues) {
    for (auto input : this->previousInput) {
        delete input;
    }

    this->previousInput = inputValues;
}

void Receiver::setInputWidths(const std::vector < uint8_t >& inputWidthValues) {
    this->inputWidths = inputWidthValues;
}

void Receiver::setFirstInputFlag() {
    this->firstInputFlag = true;
}