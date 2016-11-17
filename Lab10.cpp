//Lab8.cpp
#include <iostream>
#include <cstring>
#include <string>
//#include <locale>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
class Message{
	protected:
	string msg;
	public:
	Message();
	Message(string messagestr);
	~Message();
	virtual void printInfo();
};
Message::Message(){
	cout << "No string. Enter message: ";
	cin >> msg;
}
Message::Message(string messagestr){
	cout << "Message String Constructor" << endl;
	msg = messagestr;
printInfo();
}
Message::~Message(){
	cout << "Message Destructor" << endl;
}
void Message::printInfo(){
	cout << "Message: " << msg << endl;
}
class mcMessage : public Message{
	public:
	string *tran_msg;
	int index;
	mcMessage(string);//new constructor for derived class
	mcMessage();//new constructor for derived class
	~mcMessage();
	void translate();
	void printInfo();//redefine printInfo function
	void displayLED();
};
mcMessage::mcMessage() : Message(){
	cout << "mcMessage empty constructor" << endl;
	int strlength = msg.length();//get string length
	tran_msg = new string[strlength];//array of proper length
	translate();
	printInfo();
}
mcMessage::mcMessage(string messagestr) : Message(messagestr){
	cout << "mcMessage constructor" << endl;
	int strlength = msg.length();
	tran_msg = new string[strlength];
	translate();
	printInfo();
}
mcMessage::~mcMessage(){
	delete[] tran_msg;
	cout << "mcMessage Desctructor" << endl;
}
void mcMessage::translate(){
	//locale loc; //needed for to lower function
	string letters = "abcdefghijklmnopqrstuvwxyz";
	string MorseCode[26] = {".-", "-...", "-.-.", "-..", ".", "..-.",
	"--.", "....", "..", ".---", "-.-", ".-..",
	"--", "-.", "---", ".--.", "--.-", ".-." ,
	"...", "-", ".._", "...-", ".--", "-..-",
	"-.--", "--.."};//array of morse code letters
	for(int i=0; i<msg.length(); i++){
		index = 0;
		//cout << "msg[i]: " << msg[i] << endl;
		for(int j=0; j<26; j++){//this loop finds out what letter message is
			if(tolower(msg[i]) == letters[j])//if match found
				index = j;//save index
			}
			tran_msg[i] = MorseCode[index];//save correct Morse Code index
			//cout << "Morse code: " << tran_msg[i] << endl;
	}
}
void mcMessage::displayLED(){
	cout <<"LED display..." << endl;
	int fd;		// for the file descriptor of the special file we need to open.
	unsigned long *BasePtr;		// base pointer, for the beginning of the memory page (mmap)
	unsigned long *PBDR, *PBDDR;	// pointers for port B DR/DDR

	fd = open("/dev/mem", O_RDWR|O_SYNC);	// open the special file /dem/mem
	if(fd == -1){
		printf("\n error\n");
		return;  // failed open
	}

	// We need to map Address 0x80840000 (beginning of the page)
	BasePtr = (unsigned long*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x80840000);
	if(BasePtr == MAP_FAILED){
		printf("\n Unable to map memory space \n");
		return;
	}  // failed mmap

	// To access other registers in the page, we need to offset the base pointer to reach the
	// corresponding addresses. Those can be found in the board's manual.
	PBDR = BasePtr + 1;		// Address of port B DR is 0x80840004
	PBDDR = BasePtr + 5;	// Address of port B DDR is 0x80840014
	//*PBDDR |= 0x20;

	//*PBDDR &= 0xFFFFFFFE;	// configures port B0 as input (first push button). You could use: &= ~0x01
	for(int i=0; i<msg.length(); i++){//number of letters in string
		for(int j=0; j<tran_msg[i].length(); j++){//number of . and - in letter
			if(tran_msg[i][j]== '.'){
				cout << "Red" << endl;
				*PBDDR |= 0x20;			// configures port B5 as output (Red LED)
				*PBDR |= 0x20;	// ON
				sleep(1);
				*PBDR &= ~0x20;	// OFF
				usleep(500000); //pause for .5 seconds
			}
			else if(tran_msg[i][j]== '-'){
				cout <<"Yellow" << endl;
				*PBDDR |= 0x40;			// configures port B5 as output (Yellow LED)
				*PBDR |= 0x40;	// ON
				sleep(1);
				*PBDR &= ~0x40;	// OFF
				usleep(500000); //pause for .5 seconds
			}
		}
	}
	cout << "Green" << endl;
	*PBDDR |= 0x80;			// configures port B7 as output (Green LED)
	*PBDR |= 0x80;	// ON, Green LED shows end of string
	sleep(1);	//
	*PBDR &= ~0x80;	// OFF

	close(fd);	// close the special file
}
void mcMessage::printInfo(){
	cout << endl << "Message: " << msg << endl;
	cout << "Translated Message: ";
	for(int i=0; i<msg.length(); i++){
		cout << tran_msg[i] << " ";
	}
	cout << endl << endl;
	displayLED();
}
class msgStack{
	public:
	Message **ptSt;
	int num_obj;
	msgStack();
	~msgStack();
	void printStack();//print after each push/pop
	void push(Message *obj);
	void pop();
};
msgStack::msgStack(){
	num_obj = 0;
	cout << "Stack Constructor" << endl;
	ptSt = new Message*;
	for(int i=0; i<10; i++){
		ptSt[i] = NULL;
	}
}
msgStack::~msgStack(){
	cout << "Msg Stack Destructor" << endl;
	delete[] ptSt;
}
void msgStack::push(Message *obj){
	cout << "PUSHING" << endl;
	for(int i=9; i>=0; i--){//push message to end of stack
		if(ptSt[i] == NULL){
			ptSt[i] = obj;
			return;
		}
	}
}
void msgStack::pop(){
	for(int i=0; i<10; i++){
		if(ptSt[i]!=NULL){//first element that has info
			cout << "POPPING" << endl;
			ptSt[i] = NULL;//remove message from stack
			return;
		}
	}
}
void msgStack::printStack(){
	cout << endl << "*****Printing Stack*****" << endl;
	for(int i=0; i<10; i++){
		if(ptSt[i]!=NULL){//if message is stored, print info
			ptSt[i]->printInfo();
		}
	}
}
int main(){
	cout << "Starting program........" << endl << endl;
	mcMessage obj1("Hello");//preset string
	/*mcMessage obj2; // empty constructor
	mcMessage obj3("objthree");
	mcMessage obj4("objfour");
	msgStack msgst;
	msgst.push(&obj1);
	//msgst.push(&obj2);
	msgst.push(&obj3);
	msgst.printStack();
	msgst.pop();
	msgst.push(&obj4);
	msgst.printStack();*/
	return 0;
}
