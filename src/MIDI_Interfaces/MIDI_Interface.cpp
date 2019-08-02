#include "MIDI_Interface.hpp"

MIDI_Interface::MIDI_Interface(MIDI_Parser &parser) : parser(parser) {
    setAsDefault(); // Make this the default MIDI Interface
}

MIDI_Interface::~MIDI_Interface() {
    if (getDefault() == this)
        DefaultMIDI_Interface = nullptr;
}

MIDI_Interface *MIDI_Interface::DefaultMIDI_Interface = nullptr;

void MIDI_Interface::setAsDefault() { DefaultMIDI_Interface = this; }

MIDI_Interface *MIDI_Interface::getDefault() { return DefaultMIDI_Interface; }

// -------------------------------- READING --------------------------------- //

void MIDI_Interface::update() {
    if (callbacks == nullptr)
        return;
    bool repeat = true;
    while (repeat) {
        MIDI_read_t event = read();
        repeat = dispatchMIDIEvent(event);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

bool MIDI_Interface::dispatchMIDIEvent(MIDI_read_t event) {
    switch (event) {
        case NO_MESSAGE: return false;
        case CHANNEL_MESSAGE: onChannelMessage(); return true;
        case SYSEX_MESSAGE: onSysExMessage(); return true;
        default: onRealtimeMessage(static_cast<uint8_t>(event)); return true;
    }
}

#pragma GCC diagnostic pop

void MIDI_Interface::onRealtimeMessage(uint8_t message) {
    if (callbacks)
        callbacks->onRealtimeMessage(*this, message);
}

void MIDI_Interface::onChannelMessage() {
    if (callbacks)
        callbacks->onChannelMessage(*this);
}

void MIDI_Interface::onSysExMessage() {
    if (callbacks)
        callbacks->onSysExMessage(*this);
}

// -------------------------------- SENDING --------------------------------- //

void MIDI_Interface::send(uint8_t m, uint8_t c, uint8_t d1, uint8_t d2) {
    sendCN(m, c, d1, d2, 0);
}

void MIDI_Interface::send(uint8_t m, uint8_t c, uint8_t d1) {
    sendCN(m, c, d1, 0);
}

void MIDI_Interface::sendCN(uint8_t m, uint8_t c, uint8_t d1, uint8_t d2,
                            uint8_t cn) {
    c--;             // Channels are zero-based
    m &= 0xF0;       // bitmask high nibble
    m |= 0b10000000; // set msb
    c &= 0x0F;       // bitmask low nibble
    d1 &= 0x7F;      // clear msb
    d2 &= 0x7F;      // clear msb
    cn &= 0x0F;      // bitmask low nibble
    sendImpl(m, c, d1, d2, cn);
}

void MIDI_Interface::sendCN(uint8_t m, uint8_t c, uint8_t d1, uint8_t cn) {
    c--;             // Channels are zero-based
    m &= 0xF0;       // bitmask high nibble
    m |= 0b10000000; // set msb
    c &= 0x0F;       // bitmask low nibble
    d1 &= 0x7F;      // clear msb
    cn &= 0x0F;      // bitmask low nibble
    sendImpl(m, c, d1, cn);
}

// TODO: check for invalid addresses
void MIDI_Interface::sendNoteOn(MIDICNChannelAddress address,
                                uint8_t velocity) {
    if (address)
        sendImpl(NOTE_ON, address.getChannel().getRaw(), address.getAddress(),
                 velocity, address.getCableNumber());
}
void MIDI_Interface::sendNoteOff(MIDICNChannelAddress address,
                                 uint8_t velocity) {
    if (address)
        sendImpl(NOTE_OFF, address.getChannel().getRaw(), address.getAddress(),
                 velocity, address.getCableNumber());
}
void MIDI_Interface::sendCC(MIDICNChannelAddress address, uint8_t value) {
    if (address)
        sendImpl(CC, address.getChannel().getRaw(), address.getAddress(), value,
                 address.getCableNumber());
}
void MIDI_Interface::sendPB(MIDICNChannelAddress address, uint16_t value) {
    if (address)
        sendImpl(PITCH_BEND, address.getChannel().getRaw(), value & 0x7F,
                 value >> 7, address.getCableNumber());
}
void MIDI_Interface::sendPB(MIDICNChannel address, uint16_t value) {
    sendImpl(PITCH_BEND, address.getChannel().getRaw(), value & 0x7F,
             value >> 7, address.getCableNumber());
}
void MIDI_Interface::sendPC(MIDICNChannel address, uint8_t value) {
    sendImpl(PROGRAM_CHANGE, address.getChannel().getRaw(), value,
             address.getCableNumber());
}

// -------------------------------- PARSING --------------------------------- //

MIDI_message MIDI_Interface::getChannelMessage() {
    return parser.getChannelMessage();
}

SysExMessage MIDI_Interface::getSysExMessage() const {
    return parser.getSysEx();
}

uint8_t MIDI_Interface::getCN() const { return parser.getCN(); }