#ifndef RECEIVER_TESTER_H
#define RECEIVER_TESTER_H

char* testReceiverSimpleMessage();
char* testReceiverBinaryData();
char* testReceiverEmptyMessage();
char* testReceiverEmptyMessageIntegration();
char* testReceiverNullMessage();
char* testReceiverProtocolValidation();
char* testReceiverChecksumValidation();
char* testReceiverIntegrationWithTransmitter();
char* runAllReceiverTests();

#endif
