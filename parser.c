/**********************************************
 * Id: bush0386
 *
 * Compile: gcc -Wall parser.c -o parser
 * Run: ./parser test.txt
 *
 * This program reads in a file and breaks it 
 * into tokens, strings, comments, characters, 
 * operators, keywords, numerics, and identifiers 
 * and prints the results.
 **********************************************/
#define MAXTOKEN 128
#define MAXCHAR 128
#define MAXSTRING 256
#define MAXCMT 256
#define MAXOP 8
#define MAXKEY 32
#define MAXWORD 12

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h> 

// Linked List for keywords
struct Node {
    char *words[MAXWORD];
    int key; // size of words
    struct Node* next;
}; 
typedef struct Node* NodePtr;
NodePtr head = NULL;
NodePtr current = NULL;

// Prototypes for functions
void Sorter(char *line, int size);
bool BuildComment(char *cmt, int start, int end);
char* FinishComment(char *cmt, int size)
void PrintToken(char *tok, int size);
void PrintCharacter(char *chars, int size);
void PrintString(char *str, int size);
void PrintComment(char *cmt, int size);
void PrintOperator(char *op, int size);
void PrintKey(char *key, int size);
void PrintNumeric(char *num, int size);
void PrintIdentifier(char *id, int size);
bool singlecomment(char *line, int start, int end);
bool mltcomment(char *line, int start, int end);
bool isalphabet(char c);
bool isend(char c);
bool issymbol(char c);
bool isoperator(char c);
bool isoperators(char c, char d);
bool isspecialop(char c);
bool isspecialop2(char c, char d);
int* iskey(char *line, int start, int end);
bool isspecialkey(char *key, int start, int end);
void cutstring(char *original, char *after, int start, int end);
bool IsNumeric(char *num, int size);
bool IsIdentifier(char *id, int size);
void Copy(char *a, char *b, int size);
// Linked List Prototypes
void CopyArray(char *arr1[], char *arr2[], int size);
void PrintList();
void AddNode(int key, char *data[]);
NodePtr DeleteNode();
void DeleteList();
bool isempty();
int Length();
NodePtr FindKey(int key);
NodePtr DeleteKey(int key);


int main(int argc, char *argv[]){

    // error checking
    if(argc < 2){
        printf("Error inputing file\n");
        return -1;
    }

    // variables
    FILE *fp; 
    char *line = NULL;
    char *end = NULL;
    size_t len;
    int read; // # of chars per line
    int mltcmt = 0;
    bool finish;

    // open file
    fp = fopen(argv[1], "r");

    // loop through file and get each line 
    while((read=getline(&line, &len, fp)) != -1){
        // check for multiple line comment
        if(mltcomment(line, 0, read) || mltcmt == 1){
            mltcmt = 1;
            finish = BuildComment(line, 0, read);
            // check finish
            if(finish){
                // cut line to */ and print
                // return rest of line
                end = FinishComment(line, read);
                mltcmt = 0;
                // end of line
                // read = getline(&line, &len, fp);
            }
            else
                printf("Comment: %s", line);
        }
        if(mltcmt != 1)
            Sorter(line, read);
    }

    // close file
    fclose(fp);
    // free line if not NULL
    if(line){
        free(line);
    }
    return 0;    
}

// Function to sort the string into different parts
void Sorter(char *line, int size){
    int i, z;
    int *result;
    char token[MAXTOKEN];
    char character[MAXCHAR];
    char string[MAXSTRING];
    char comment[MAXCMT];
    char operator[MAXOP];
    char key[MAXKEY];
    static int tok_count;
    static int char_count;
    static int str_count;
    static int cmt_count;
    static int op_count;
    static int key_count;
    char char_flag = '\'';
    char str_flag = '\"';


    for(i=0; i<size; i=i)    
    {   
        //printf("Line[%d] = %c\n", i, line[i]);
        // set values to 0
        if(i==0){
            tok_count = char_count = str_count = cmt_count = op_count = key_count = 0 ;
        }
        // check for keywords
        result = iskey(line, i, size);
        if(result[0] == 1){
            for(z=0; z<result[1]; z++){
                key[key_count] = line[i];
                key_count++;
                i++;
            }
            PrintKey(key, key_count);
            key_count = 0;
        }
        // Check if its a char
        else if(line[i] == char_flag){
            do{
                character[char_count] = line[i];
                char_count++;
                i++;
            }while(line[i] != char_flag);
            if (line[i+1] == char_flag){
                character[char_count] = '\'';
                char_count++;
                i++;
            }
            character[char_count] = '\'';
            PrintCharacter(character, char_count+1);
            char_count = 0;
            character[char_count] = '\0';
            i++;
        }
        // Check if its a string 
        else if(line[i] == str_flag){
            do{
                string[str_count] = line[i];
                str_count++;
                i++;
            }while(line[i] != str_flag);
            string[str_count] = '\"';
            PrintString(string, str_count+1);
            str_count = 0;
            string[str_count] = '\0';
            i++;            
        }
        // check for single comment
        else if(singlecomment(line, i, size)){
            do{
                comment[cmt_count] = line[i];
                cmt_count++;
                i++;
            }while(!(line[i] == '*' && line[i+1] == '/'));
            comment[cmt_count] = '*';
            comment[cmt_count+1] = '/';
            i += 2;
            PrintComment(comment, cmt_count+2);
            cmt_count = 0;
            comment[cmt_count] = '\0';
        }
        // check for operator
        else if(isoperators(line[i], line[i+1])){
            //check if token needs to be printed
            if(tok_count > 0){
                if(IsNumeric(token, tok_count))
                {
                    PrintNumeric(token, tok_count);
                    tok_count = 0;
                    token[tok_count] = '\0';
                }
                else if(IsIdentifier(token, tok_count)){
                    PrintIdentifier(token, tok_count);
                    tok_count = 0;
                    token[tok_count] = '\0';
                }
                else{
                    PrintToken(token, tok_count);
                    tok_count = 0;
                    token[tok_count] = '\0';
                }
            }
            // check special cases
            if(isspecialop(line[i])){
                if(isspecialop2(line[i], line[i+1])){
                    operator[op_count] = line[i];
                    operator[op_count+1] = line[i+1];
                    i += 2;
                    PrintOperator(operator, 2);
                    op_count = 0;
                }
                else{
                    operator[op_count] = line[i];
                    i++;
                    PrintOperator(operator, 1);
                    op_count = 0;
                }
            }
            else{
                // check if its single or double operator
                if(line[i] == '.' || line[i] == '!'){
                    operator[op_count] = line[i];
                    operator[op_count+1] = line[i+1];
                    PrintOperator(operator, 2);
                    op_count = 0; 
                    i += 2;
                }
                else{
                    operator[op_count] = line[i];
                    PrintOperator(operator, 1);
                    op_count = 0;
                    i++;
                }
                
            }
        }
        // It is a token, numeric, or identifier and print
        else if(isend(line[i])){
            if(IsNumeric(token, tok_count))
            {
                PrintNumeric(token, tok_count);
                tok_count = 0;
                token[tok_count] = '\0';
                i++;
            }
            else if(IsIdentifier(token, tok_count)){
                PrintIdentifier(token, tok_count);
                tok_count = 0;
                token[tok_count] = '\0';
                i++;               
            }
            else{
                PrintToken(token, tok_count);
                tok_count = 0;
                token[tok_count] = '\0';
                i++;
            }
        }
        else{
            // check for identifier
            token[tok_count] = line[i];
            tok_count++;
            i++;
        }
    }
}
// Function to build multiple line comment
bool BuildComment(char *cmt, int start, int end){
    int complete = 0;
    //printf("Start: %d\tEnd: %d\n", start, end);
    do{
        if(cmt[start] == '*' && cmt[start+1] == '/'){
            complete = 1;
            break;
        }
        start++;
        if(start > end)
            break;
    }while(cmt[start] != '\n');

    if(complete == 1){
        return true;
    }
    else{
        return false;
    }
}
// Function to print off token
void PrintToken(char *tok, int size){
    if(size !=0 ){
        tok[size] = '\0';
        printf("Token: %s\n", tok);
    }
    return;
}
// Function to print off char
void PrintCharacter(char *chars, int size){
    if(size !=0 ){
        chars[size] = '\0';
        printf("Char: %s\n", chars);
    }
    return;
}
// Function to print off string
void PrintString(char *str, int size){
    if(size !=0 ){
        str[size] = '\0';
        printf("String: %s\n", str);
    }
    return;
}
// Function to print off single comment
void PrintComment(char *cmt, int size){
    if(size !=0 ){
        cmt[size] = '\0';
        printf("Comment: %s\n", cmt);
    }
    return;
}
// Function to print off single comment
void PrintOperator(char *op, int size){
    if(size !=0 ){
        op[size] = '\0';
        printf("Operator: %s\n", op);
    }
    return;
}
// Function to print off keyword
void PrintKey(char *key, int size){
    if(size !=0 ){
        key[size] = '\0';
        printf("Keyword: %s\n", key);
    }
    return;
}
// Function to print off numeric
void PrintNumeric(char *num, int size){
    if(size !=0 ){
        num[size] = '\0';
        printf("Numeric: %s\n", num);
    }
    return;
}
// Function to print off identifier
void PrintIdentifier(char *id, int size){
    if(size !=0 ){
        id[size] = '\0';
        printf("Identifier: %s\n", id);
    }
    return;
}
// Function to find single comment
bool singlecomment(char *line, int start, int end){
    int i;
    if(line[start] == '/' && line[start+1] == '*'){
        for(i = start+2; i<end; i++){
            if (line[i] == '*' && line[i+1] == '/')
                return true;
        }
    }
    return false;
}
// Function to find multiple line comment
bool mltcomment(char* line, int start, int end){
    int i;
    for(i=start; i<end; i++)
        if(line[start] == '/' && line[start+1] == '*'){
            if(singlecomment(line, start, end))
                return false;
            else
                return true;
        }
    return false;
}
// Function to check if character is a digit or alphabetic
bool isalphabet(char c){
    return (isalpha(c) || isdigit(c) || c == '_');
}
// Function to check if character is a non operator symbol
bool issymbol(char c){
    return (c == '?' || c == '~' || c == '^' || c == '#'  );
}
// Function to check if at end of token
bool isend(char c){
    return (c == '\n' || c == '\t' || isspace(c) || c == EOF);
}
// Function to check if character is operator
bool isoperator(char c){
    char ops[20] = { '<', '>', '(', ')', '+', '-', '*', '/', '|', '&', ';', 
    ',', ':', '=', '$', '@', '[', ']', '{', '}'} ;
    int i;
    for(i=0; i<20; i++){
        if(c == ops[i])
            return true;
    }
    return false;

}
// Function to check if character is operator
bool isoperators(char c, char d){
    char ops[21] = { '<', '>', '(', ')', '+', '-', '*', '/', '|', '&', ';', 
    ',', ':', '=', '$', '@', '[', ']', '{', '}'} ;
    char* ops2[2] = { "..", "!="};
    int i, z;
    z = 0;
    for(i=0; i<21; i++){
        if(c == ops[i])
            return true;
        if(c == ops2[z][0] && d == ops2[z][1])
            return true;
        if(z<1)
            z++;
    }
    return false;

}
// Function to check if character is special operator
bool isspecialop(char c){
    char special[7] = { '<', '>', '{', '}', '*', ':', '=' } ;
    int i;
    for(i=0; i<7; i++){
        if(c== special[i])
            return true;
    }
    return false;
}
// Function to check if its 2 character operator
bool isspecialop2(char c, char d){
    char* special[10] = { "**", ":=", "<<", ">>", "<>", "<=", ">=", "=>", "{:", "}:" };
    int i;
    for(i=0; i<10; i++){
        if(c == special[i][0] && d == special[i][1])
            return true;
    }
    return false;
}
// check if key word is found
int* iskey(char *line, int start, int end){
    char *key_2[5] = { "if", "in", "is", "of", "or"};
    char *key_3[6] = { "and", "end", "ifc", "out", "pkg", "ptr"};
    char *key_4[10] = { "bool", "case", "else", "exit", "func", "null", "proc", "then", "type", "when"};
    char *key_5[5] = { "array", "elsif", "float", "range", "while"};
    char *key_6[1] = { "others" };
    char *key_7[4] = { "integer", "mutator", "natural", "subtype"};
    char *key_8[3] = { "accessor", "constant", "positive"};
    char *key_9[1] = {"character"};
    char check[10];
    static int val[2] = {0, 0}; // val[0] = word found (0 or 1) // val[1] = size
    int size, t1, t2, i;
    NodePtr p;

    t1 = t2 = 0;
    size = 9;
    
    // Add arrays to a linked list
    AddNode(5, key_2);
    AddNode(6, key_3);
    AddNode(10, key_4);
    AddNode(5, key_5);    
    AddNode(1, key_6);
    AddNode(4, key_7);
    AddNode(3, key_8);
    AddNode(1, key_9);

    //printf("Original List: \n");
    //PrintList();

    size = 9;
    // check for words
    for(p = head; p != NULL; p = p->next){
    if(size <= end-start){    
        cutstring(line, check, start, size+start);
        for(i = 0; i < p->key; i++){
            if(strncmp((p->words)[i], check, size) == 0){
                // check if at beginning of line
                if(start == 0)
                    t1 = 1;
                //check for other conditions before
                switch(line[start-1]){
                    case ' ':
                        t1 = 1;
                        break;
                    case '\n':
                        t1 = 1;
                        break;
                    case '\t':
                        t1 = 1;
                        break;
                }
                if(isoperator(line[start-1])){
                    t1 = 1;
                }
                // check for other conditions after
                switch(line[size+start]){
                    case ' ':
                        t2 = 1;
                        break;
                    case '\n':
                        t2 = 1;
                        break;
                    case '\t':
                        t2 = 1;
                        break;
                }
                if(end == (start+size+2))
                    t2 = 1;
                if(isoperator(line[start+size])){
                    t2 = 1;
                }
                if(t1 == 1 && t2 == 1){
                    val[0] = 1;
                    val[1] = size;
                    return val;
                }
            }  
            t1 = t2 = 0;   
        }
    }
    size--;
    }
    DeleteList();

    // end 
    val[0] = 0;
    val[1] = 0;
    return val;
}
// Function to cut string at certain point and copy to new string
void cutstring(char *original, char *after, int start, int end){
    int i;
    int j;
    j = 0;
    for(i=start; i<end; i++){
        after[j] = original[i];
        j++;
    }
    after[j] = '\0';
}
// Function to check if string is numeric
bool IsNumeric(char *num, int size){
    int special, i, j; // '.'
    char copy[24];
    special = j = 0;
    Copy(num, copy, size);
    for(i = 0; i<size;){
        if(isxdigit(copy[i]))
            j++;
        else if(copy[i] == '.' && special == 0 && size > 1){
            j++;
            special = 1;
        }
        i++;
    }
    //printf("this is i= %d, this is j= %d\n", i, j);
    return (i==j);

}
// Function to check if string is an identifier
bool IsIdentifier(char *id, int size){
    int i, j, first;
    char copy[24];
    j = first = 0;
    Copy(id, copy, size);
    for(i = 0; i<size;){
        if(isalpha(copy[i])){
            if(i == 0){
                first = 1;
            }
            j++;
        }
        else if(isdigit(copy[i])){
            j++;
        }
        else if(copy[i] == '_'){
            j++;
        }
        i++;
    }
    //printf("this is i= %d, this is j= %d\n", i, j);
    return (i==j && first);    
}
// copy string to another string
void Copy(char *a, char *b, int size){
    int i;
    for(i=0; i<size; i++){
        b[i] = a[i];
    }
    return;
}
// Function to copy one array into another
void CopyArray(char *arr1[], char *arr2[], int size){
    int i;
    for(i=0; i < size; i++){
        arr2[i] = arr1[i];
        //printf("arr1[%d] = %s\n", i, arr1[i]);
    }
    return;
}
// print list
void PrintList(){
    NodePtr p;
    char *tmp[MAXWORD];
    int i;
    i = 0;
    p = head;
    while(p != NULL){
        CopyArray(p->words, tmp, p->key);
        printf("Key-%d, Words: " , p->key);
        while(i < p->key){
            printf("%s ", tmp[i]);
            i++;
        }
        i = 0;
        printf("\n");
        p = p->next;
    }
 
    return;
}
//add node to front of list
void AddNode(int key, char *data[]) {
   //allocate
   NodePtr p = (NodePtr) malloc(sizeof(struct Node));
	
   //intialize
   p->key = key;
   
   CopyArray(data, p->words, key);
	
   //point to old head
   p->next = head;
	
   // make p new head
   head = p;
}
//delete first node
NodePtr DeleteNode() {

   //save head node
   NodePtr temp = head;
	
   //set head to next node 
   head = head->next;
	
   //return removed node
   return temp;
}
// delete list
void DeleteList(){
    NodePtr tmp;
    while(head != NULL){
        tmp = head->next;
        free(head);
        head = tmp;
    }
    head = NULL;

}
//check if list is empty
bool isempty() {
   return head == NULL;
}
// find length of list
int Length() {
   int length = 0;
   NodePtr current;
	
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
	
   return length;
}
// find node with key
NodePtr FindKey(int key) {

   //start from the first link
   NodePtr current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if data found, return the current Link
   return current;
}
//delete a node with key
NodePtr DeleteKey(int key) {

   //start from the first link
   NodePtr current = head;
   NodePtr previous = NULL;
	
   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->key != key) {

      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //store reference to current link
         previous = current;
         //move to next link
         current = current->next;
      }
   }

   //found a match, update the link
   if(current == head) {
      //change first to point to next link
      head = head->next;
   } else {
      //bypass the current link
      previous->next = current->next;
   }    
	
   return current;
}
