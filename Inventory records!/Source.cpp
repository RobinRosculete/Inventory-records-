#include <iostream>
#include<string>
#include<fstream>
#include <iomanip>

using namespace std;

const int ARRAY_SIZE = 100;
const int ITEM_ID_SIZE = 5;
const int WIDTH_SIZE = 10;

enum Status {DEFAULT_STATUS, ACTIVE, DISCONTINUED, RECALLED };
enum MenuOptions { DEFAULT_MENU, PRINT_ALL, PRINT_ERROR, SEARCH, DETAILS,CHAGE_STATUS, PRINT_ACTIVE, PRINT_DISCONTINUED, PRINT_RECALLED ,SORT, QUIT };
enum SortMenuCase{DEFAUL_SORT,BY_ID, BY_NAME, BY_QUANTITY,BY_PRICE};

struct Record {
	string itemID;
	string itemName;
	int itemQuantity;
	float itemPrice;
	Status itemStatus;
};

void readFile(Record recordArray[], int& countItems, int& countErrors);
void print(const Record printData[], int countItems, Status itemStatus);
void printInvalid(int countErrors);
void menu(int& optionNum);
bool isValidRec(ofstream& errorFile, Record temp);
bool isAlphaCharater(ofstream& errorFile, string itemName);
int searchFunc(const Record recArray[], string itemName, int countItems);
void printFoundFunc(const Record recordArray[], int count);
string toUppercase(string& itemName);
float calcTotalWorth(const Record recordArray[], int countItems);
int countTotalItems(const Record recordArray[], int countItems);
void changeStatusMenu(Record reccordArray[], int countItems);
void changeStatus(Record recordArray[], int statusOption, int itemSubscript);

void Sort(Record* ptrRecordArray[], int optionNum, int countItems);
void SortMenu(Record* ptrRecordArray[], int countItems);
void printSortedRecord(Record* ptrRecordArray[], int countItems);


int main() {
	int optionNum = DEFAULT_MENU;
	int countItems = 0;
	int countErrors = 0;
	Record recordArray[ARRAY_SIZE];
	Record*  ptrRecordArray[ARRAY_SIZE]{ nullptr };

	cout << setprecision(2)
		<< fixed;

	readFile(recordArray, countItems, countErrors);

	for (int x = 0; x < countItems; x++) {
	ptrRecordArray[x] = &recordArray[x];
	}
	while (optionNum != QUIT) {
		menu(optionNum);
		switch (optionNum)
		{
		case PRINT_ALL: print(recordArray, countItems,DEFAULT_STATUS);
			break;                                                     

		case PRINT_ERROR: printInvalid(countErrors);
			break;

		case SEARCH: printFoundFunc(recordArray, countItems);
			break;

		case CHAGE_STATUS:changeStatusMenu(recordArray, countItems);
			break;

		case PRINT_ACTIVE:print(recordArray, countItems,ACTIVE);
			break;

		case PRINT_DISCONTINUED:print(recordArray, countItems, DISCONTINUED);
			break;

		case PRINT_RECALLED:print(recordArray, countItems, RECALLED);
			break;

		case DETAILS: cout << "The number of unique Items in the inventory is: " << countItems + countErrors
				           << "\nThe total worth of the inventory is: " << calcTotalWorth(recordArray, countItems)
				           << "\nThe total count of all items in the inventory " << countTotalItems(recordArray, countItems) << endl;
			break;
		case SORT:SortMenu(ptrRecordArray, countItems);
			break;

		case QUIT: cout << "Quiting program..." << endl;
			break;

		default: cout << "Invlaid input. Please renter an option from bellow " << endl;
			break;
		}

	}
}// end main funciton

//Function role: To print a menu of options to the screen, and return the option number.
void menu(int& optionNum) {

	cout << "\n\tPlease enter an option from bellow"
		"\n\t1.Print all items in the inventory unsorted!\n"
		"\t2.Print invalid records from an error file!\n"
		"\t3.Search for an item by ID or name!\n"
		"\t4.Print a report of details!\n"
		"\t5.Change Status of Item!\n"
		"\t6.Print all active items\n"
		"\t7.Print all discontinued items\n"
		"\t8.Print all recalled Items\n"
		"\t9.Sort by any field in ascending order\n"
		"\t10.Quit Program!" << endl;

	//Checking if the user enetred an int, not a char or string
	if (!(cin >> optionNum)) {
		cin.clear();
		cin.ignore(2000, '\n');
	}
}// End menu Funciton!

//Function role to read the file, and add the corect input into the array
void readFile(Record recordArray[], int& countItems, int& countErrors) {
	Record temp;
	fstream inFile;
	ofstream errorFile;
	temp.itemStatus = ACTIVE;
	inFile.open("InFile.txt");

	if (inFile.fail()) {

		cout << "Error inFile could not be opened. Quitingprogram" << endl;
		exit(EXIT_FAILURE);
	}//End Error File check

	//Checking if file is empty
	if (inFile.fail() || inFile.peek() == ofstream::traits_type::eof()) {

		cout << "The input file is empty. Quitting the program" << endl;
		exit(EXIT_FAILURE);

	}
	errorFile.open("OutFile.txt");

	if (errorFile.fail())
	{
		cout << "Error outFile could not be opened. Quitingprogram" << endl;
		exit(EXIT_FAILURE);
	}
	errorFile << "***Error File***" << endl;

	while (!inFile.eof() && countItems < ARRAY_SIZE) {
	 inFile >> temp.itemID
			>> temp.itemName
			>> temp.itemQuantity
			>> temp.itemPrice;

		toUppercase(temp.itemID);
		toUppercase(temp.itemName);

		if (isValidRec(errorFile, temp)) {
			recordArray[countItems] = temp;
			countItems++;
		}else countErrors++;
	}//end while loop


	if (countItems == ARRAY_SIZE && !inFile.eof()) {

		cout << "***WARNING!***\nNot all records have been stored; only first " <<
			ARRAY_SIZE << " will be processed" << endl;

	}
	if (countErrors != 0 && countItems == 0) {
		cout << "***WARNING!***\n There are no valid records! Please check Error File. " << endl;
	}

	inFile.close();
	errorFile.close();
}//End ReadFile function

// Function role: To check if an item is valid or not, and print a coresponding error message!
bool isValidRec(ofstream& errorFile, Record temp)
{
	bool isValid = true;

	if (temp.itemID.size() != ITEM_ID_SIZE || !isAlphaCharater(errorFile, temp.itemID))
	{
		errorFile << setw(WIDTH_SIZE) << temp.itemID
			<< setw(WIDTH_SIZE) << temp.itemName
			<< setw(WIDTH_SIZE) << temp.itemQuantity
			<< setw(WIDTH_SIZE) << temp.itemPrice << "\t Error! the item ID is invalid!" << endl;
		isValid = false;
	}
	else if (!isAlphaCharater(errorFile, temp.itemName)) {

		errorFile << setw(WIDTH_SIZE) << temp.itemID
			<< setw(WIDTH_SIZE) << temp.itemName
			<< setw(WIDTH_SIZE) << temp.itemQuantity
			<< setw(WIDTH_SIZE) << temp.itemPrice << "\t Error! the item Name is invalid!" << endl;
		isValid = false;
	}
	else if (temp.itemQuantity < 0) {
 errorFile << setw(WIDTH_SIZE) << temp.itemID
			<< setw(WIDTH_SIZE) << temp.itemName
			<< setw(WIDTH_SIZE) << temp.itemQuantity
			<< setw(WIDTH_SIZE) << temp.itemPrice << "\t Error! the item Quantity is invalid!" << endl;
		isValid = false;
	}
	else if (temp.itemPrice < 0) {
		errorFile << setw(WIDTH_SIZE) << temp.itemID
			<< setw(WIDTH_SIZE) << temp.itemName
			<< setw(WIDTH_SIZE) << temp.itemQuantity
			<< setw(WIDTH_SIZE) << temp.itemPrice << "\t Error! the item Price is invalid!" << endl;
		isValid = false;
	}

	return isValid;
}//End Function isValidRec()

//Function role:Checking if item name is only alpha charaters
bool isAlphaCharater(ofstream& errorFile, string itemName) {
	bool isAlpha = true;
	for (int x = 0; itemName[x] != '\0'; x++) {

		if (!isalnum(itemName[x])) {
			isAlpha = false;
		}
	}
	return isAlpha;
}//end isAlphavharacters

//Function role to output on to the screen all items|| Active items|| Discontinued Items||Recalled Items
void print(const Record recordArray[], int count, Status itemStatus) {
    if (count == 0) {
        cout << "There are no valid Records! Please check Error File! " << endl;
    }
        for (int x = 0; x < count; x++) {
            if (itemStatus == DEFAULT_STATUS) {
                cout << setw(WIDTH_SIZE) << recordArray[x].itemID
                    << setw(WIDTH_SIZE) << recordArray[x].itemName
                    << setw(WIDTH_SIZE) << recordArray[x].itemQuantity
                    << setw(WIDTH_SIZE) << recordArray[x].itemPrice
                    << endl;
            }else if (recordArray[x].itemStatus == itemStatus) {
                cout << setw(WIDTH_SIZE) << recordArray[x].itemID
                    << setw(WIDTH_SIZE) << recordArray[x].itemName
                    << setw(WIDTH_SIZE) << recordArray[x].itemQuantity
                    << setw(WIDTH_SIZE) << recordArray[x].itemPrice
                    << endl;
            }
        }//end for loop

}//End Print Function

//Function role to print to the screen the invalid records from the error file
void printInvalid(int countErrors) {
	ifstream errorFile;
	string temp;

	errorFile.open("OutFile.txt");
	if (errorFile.fail())
	{
		cout << "There was an error, can't find error file!" << endl;
		exit(EXIT_FAILURE);
	}

	if (countErrors == 0) {

		cout << "There are no invalid records!" << endl;
	}
	else {

		while (!errorFile.eof()) {
			getline(errorFile, temp);
			cout << temp << endl;
		}

	}//End Else Statement!
	errorFile.close();
}//End function PrintInvalid()

//Function purpose to print the serched function
void printFoundFunc(const Record recordArray[], int countItems) {
	string itemName;
	int itemSubscript = 0;

	cout << "Enter an Item ID or Item Name: ";
	cin >> itemName;
	toUppercase(itemName);

	itemSubscript = searchFunc(recordArray, itemName, countItems);
	if (itemSubscript > countItems-1) {

		cout << "Your Item was not found! Please try again." << endl;

	}
	else {
	  cout << setw(WIDTH_SIZE) << recordArray[itemSubscript].itemID
			<< setw(WIDTH_SIZE) << recordArray[itemSubscript].itemName
			<< setw(WIDTH_SIZE) << recordArray[itemSubscript].itemQuantity
			<< setw(WIDTH_SIZE) << recordArray[itemSubscript].itemPrice
			<< endl;
	}

}//End printFoundFunction

 //Function role to search for an Item in the inventory and return the subscript;
int searchFunc(const Record recordArray[], string itemName, int count) {
	int itemSubscript = -1;
	bool found = false;

	while (itemSubscript < count && !found)
	{
		if (recordArray[itemSubscript].itemID == itemName || recordArray[itemSubscript].itemName == itemName)
		{
			found = true;
		}
		else
		{
			++itemSubscript;
		}
	}

	return itemSubscript;
}// end search function


//Function role to make a string upercase and return it
string toUppercase(string& itemName) {

	for (int x = 0; x < itemName.length(); x++) {
		itemName[x] = toupper(itemName[x]);
	}
	return itemName;  
}//end Uppercase function

//Function purpose to calculate the total worth of the inventory and return it
float calcTotalWorth(const Record recordArray[], int countItems) {
	float totalWorth = 0;

	for (int x = 0; x < countItems; x++) {

		totalWorth += (recordArray[x].itemQuantity * recordArray[x].itemPrice);
	}
	return totalWorth;
}//end calcTotatlWorth() 

//Function role to count the total of items and return it!
int countTotalItems(const Record recordArray[], int countItems) {
	int numItems = 0;

	for (int x = 0; x < countItems; x++) {

		numItems += recordArray[x].itemQuantity;
	}
	return numItems;
}//End countTotalItems()
                 
//function purpose to sort the record by any field
void Sort(Record* ptrRecordArray[], int optionNum, int countItems) {
    Record* temp = nullptr;//Using variable to temporarly
	bool option = false;

			for (int y = 0; y < countItems - 1; y++) {
				for (int z = 0; z < countItems - y - 1; z++) {

					switch (optionNum) {
					case BY_ID:	option = (ptrRecordArray[z]->itemID > ptrRecordArray[z + 1]->itemID);
						break;

					case BY_NAME: option = (ptrRecordArray[z]->itemName > ptrRecordArray[z + 1]->itemName);
						break;

					case BY_QUANTITY: option = (ptrRecordArray[z]->itemQuantity > ptrRecordArray[z + 1]->itemQuantity);
						break;

					case BY_PRICE: option = (ptrRecordArray[z]->itemPrice > ptrRecordArray[z + 1]->itemPrice);
						break;
			
					default:cout << "Invlaid input. Please Renter: " << endl;
						break;
					}
					if (option) {
                     
                        temp = ptrRecordArray[z];
                        ptrRecordArray[z] = ptrRecordArray[z + 1];
                        ptrRecordArray[z + 1] = temp;
						//swap(ptrRecordArray[z],ptrRecordArray[z + 1]);	
					}//end id statement option

				}//End for loop z
			}// End for loop y			

}//End Sort()

//Submenu to sort by various fileds
void SortMenu(Record* ptrRecordArray[], int countItems) {
    const int returnToMenu = 5;
    int optionNum = 1;
cout << "By which field would you like to sort the fucntion?\n"
	     "1.By ID\n"
		 "2.By Name\n"
		 "3.By Quantity\n"
		 "4.By price\n"
		 "5.Return to main menu" << endl;
	//Checking if the user enetred an int, not a char or string
	while (!(cin >> optionNum) || optionNum <= 0 || optionNum > 5) {
		cout << "Wrong input, please enter a option from 1 to 4!" << endl;
		cin.clear();
		cin.ignore(2000, '\n');
	}
    if (optionNum == returnToMenu) cout << "Returned to menu!" << endl;
    else {
        Sort(ptrRecordArray, optionNum, countItems);
        printSortedRecord(ptrRecordArray, countItems);
    }
}// End SortMenu()

//Function purpose to print the sorted Array 
void printSortedRecord(Record* ptrRecordArray[], int countItems)
{    for (int x = 0; x < countItems; x++) {
          cout << setw(WIDTH_SIZE) << ptrRecordArray[x]->itemID
               << setw(WIDTH_SIZE) << ptrRecordArray[x]->itemName
               << setw(WIDTH_SIZE) << ptrRecordArray[x]->itemQuantity
               << setw(WIDTH_SIZE) << ptrRecordArray[x]->itemPrice << endl;
    }
}//End printSortedRecord()

//Funciton purpose to ask user for item an print the change status options for that item
void changeStatusMenu(Record recordArray[], int countItems) {
    const string backToMenu = "4";
	int statusOption=DEFAULT_STATUS;
	int itemSubscript=0;
    string statusArray[] = { "DEFAULT_STATUS", "ACTIVE", "DISCONTINUED", "RECALLED" };
	string item;
   
	cout << "What item do you want to change the status of? (If you wnat to go back to the main Menu press '4') " << endl;
	cin >> item;
	toUppercase(item);
    itemSubscript = searchFunc(recordArray, item, countItems);

    //Checking if the item exists in the record by checking if the itemSubscript is greater than the num of members in Record
    while (itemSubscript > countItems - 1 && item !="4") {
        cout << "This item does not exist in our Record! Please try a different one! If you have a" << endl;
        cin >> item;
        toUppercase(item);
        itemSubscript = searchFunc(recordArray, item, countItems);
    }

    if (item == backToMenu) cout << "Returned to main menu" << endl;
    else {
        cout << "\tYour Item Status is currently " << statusArray[recordArray[itemSubscript].itemStatus] <<
            "\n\tTo what status do you want to change it" << endl;

        if (recordArray[itemSubscript].itemStatus != ACTIVE)  cout << "\tActive press'1'" << endl;
        if (recordArray[itemSubscript].itemStatus != DISCONTINUED) cout << "\tDiscontinued press '2'" << endl;
        if (recordArray[itemSubscript].itemStatus != RECALLED) cout << "\tRecalled press '3'" << endl;

        cout << "\tGo back to main Menu press '4'" << endl;
        cin >> statusOption;

        changeStatus(recordArray, statusOption, itemSubscript);
    }
}//End Void Change Status

//Function purpuse to change the status of an item!
void changeStatus(Record recordArray[],int statusOption, int itemSubscript) {

	if (statusOption != 4)
	{
		switch (statusOption)
		{
		case  ACTIVE:recordArray[itemSubscript].itemStatus = ACTIVE;
			cout << "Your Item was changed to ACTIVE status!" << endl;
			break;
		case DISCONTINUED:recordArray[itemSubscript].itemStatus = DISCONTINUED;
			cout << "Your Item was changed to DISCONTINUED status!" << endl;
			break;
		case RECALLED:recordArray[itemSubscript].itemStatus = RECALLED;
			cout << "Your Item was changed to RECALLED status!" << endl;
			break;
		default:cout << "This status does not exist! Please try again!" << endl;

			break;
		}
	}//end 	if (statusOption != 4)
}//End changeStatus()