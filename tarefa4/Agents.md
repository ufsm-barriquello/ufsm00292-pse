# Building
run `make all` in the tarefa2 folder
then run the test executable to see if it worked

# State machine
The state machine is a reader that processes one byte at a time, and a proper message takes the format START(1 byte, value 0x02) | QUANTITY (1 Byte) | DATA (Quantity Bytes) | CHECKSUM (1 Byte, should match a calculated checksum) | End (1 Byte, 0x03)


# Impletmentation
* Touch the tests to use the structs you define
* Use protothreads (after further requirement analysis, it should use a event driven design)
* Calculate checksum starting with 0 and XOR incoming data 
* If checksum doesn't match go to START

# Extra
* Ask for clarification if needed
