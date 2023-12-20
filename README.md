# SIC Assembler實作

## Symbol Table結構

```c
    struct SymbolEntry {
      char l[7];
      int v;
      struct SymbolEntry *next, *prev;
    };
    
    struct SymbolTable {
        struct SymbolEntry* head;
        struct SymbolEntry* tail;
    };
```
>> 輸出範例
label(l) 為7個字元的字元陣列，Locctr(v)為整數，但印出時會轉成十六進制。
```
Symbol
name      hex              dec  scope     kind      type      description
FIRST     0000000000         0  local     relative  data      label     
CLOOP     0000000006         6  local     relative  data      label     
ENDFIL    000000001A        26  local     relative  data      label     
EOF       000000002D        45  local     relative  data      label     
RETADR    0000000030        48  local     relative  data      label     
LENGTH    0000000033        51  local     relative  data      label     
BUFFER    0000000036        54  local     relative  data      label     
RDREC     0000001036      4150  local     relative  data      label     
RLOOP     0000001040      4160  local     relative  data      label     
EXIT      0000001056      4182  local     relative  data      label     
INPUT     000000105C      4188  local     relative  data      label     
WRREC     000000105D      4189  local     relative  data      label     
WLOOP     0000001062      4194  local     relative  data      label     
OUTPUT    0000001076      4214  local     relative  data      label
```

### Symbol Table 操作
```c
  int insertSymbol(char s[7], int r);
  int findSymbol(char *s);
  void printSymbolTable();
```


## Object Code 
```c
  struct ObjectCode
  {
      unsigned int format;
      unsigned int obj_code;
      unsigned int byte_length;
      unsigned int address;
  
      struct ObjectCode *next;
  };
  
  struct OBJECT_CODE_LIST
  {
      unsigned int top;
      struct ObjectCode *head;
      struct ObjectCode *tail;
  };
```
將 object code 得值使用整數儲存
一個整數 1 word = 4 bytes = 32 bits 剛好可以裝下一個 formt 4 的 object code

### Object Code List

```c
  struct ObjectCode *createObjectCode(unsigned int address);
  
  void init_op_code(struct ObjectCode *o, char *m);
  
  void init_address_mode(struct ObjectCode *o,unsigned int am);
  
  void init_registers(struct ObjectCode *o,int rg);
  
  void init_disp(struct ObjectCode *o, int rg);
  
  void init_disp4(struct ObjectCode *o, int rg);
  
  
  int init_byte(struct ObjectCode *o, char c);

  void init_hex_byte(struct ObjectCode *o, int b);
    
  void insert_object_list(struct ObjectCode *o);
  
  int is_format_two(const char *op);
```

使用 bitwise operation 將對應的欄位依序填入
建立好的 object code 物件先存入 object list 待之後印出

根據課本Fig.2.4的Two-Pass演算法中翻譯成C語言的程式
```c
  int readline();
  
  void ADD_LOCTR();
    
  void pass_one();
  
  void pass_two();
  
  void generateObjectFile();
```

pass one

建立 symbol table
```c
  void pass_one() {
      //
      //  pass one
      //
      printf("pass one...\n");
      while (readline())
      {
          if(LINE_NOT_EMPTY){
              ADD_LOCTR();
              if(findSymbol(label)){
                  // printf("%s exist in symbol table\n", label);
              }
              else if(label[0]!='\0'){
                  insertSymbol(label,pg_loc);
                  // printf("insert %s into symbol table\n", label);
              }
              // write_line(fd,NULL);
          }
      }
      printSymbolTable();
  }
```
pass two

產生 object file 
```c
  void pass_two() {
      //
      //  pass two
      //
      // print_line = 1;
      const char *filename = "output.lst";
      int fd;
      start_loc = 0;
      locctr = 0;
      pg_loc = 0;
      fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  
      if (fd == -1) {
          perror("Error opening file");
          exit(EXIT_FAILURE);
      }
   
      while (readline())
      {
          if(LINE_NOT_EMPTY){
              ADD_LOCTR();
              // set base register
              if (CMP(op, D_BASE))
                  base = findSymbol(operand1);
              ///
              struct ObjectCode *obj = NULL;  /// initialize object node
              size_t base_relative = 0;
              if (!(CMP(op, D_START) || CMP(op, D_RESW) || CMP(op, D_RESB) || CMP(op, D_WORD) || CMP(op, D_BASE) || CMP(op, D_END)))
              {
                  obj = createObjectCode(pg_loc);
                  INIT_OBJECT_CODE(obj);
  
                  if (CMP(op, "BYTE"))
                  {
                      char bytes[MAX_LABEL_LENGTH];
                      char *start = operand1 + 2;
                      char *end = operand1 + 2;
  
                      if (operand1[0] == 'C')
                      {
                          EXTRACT_operand1(end);
                          strncpy(bytes, start, STR_LEN(start, end));
                          bytes[STR_LEN(start, end)] = '\0';
                          obj->byte_length = (STR_LEN(start, end)) / 4 + 1;
                          for (; start <= end; start++)
                          {
                              while (!init_byte(obj, *start))
                              {
                                  insert_object_list(obj);
                                  obj = createObjectCode(pg_loc);
                                  INIT_OBJECT_CODE(obj);
                              }
                      #ifdef PASS_TWO_DEBUG
                          printf("OBJECT CODE : [%06X]\n", obj->obj_code);
                      #endif
                      }
                  }
                  else if (operand1[0] == 'X')
                  {
                      EXTRACT_operand1(end);
                      strncpy(bytes, start, STR_LEN(start, end));
                      bytes[STR_LEN(start, end)] = '\0';
                      int byte = strtol(bytes, NULL, 16);
                      init_hex_byte(obj, byte);
                  }
                  #ifdef PASS_TWO_DEBUG
                      printf("extract string %s\n", bytes);
                  #endif
              }
              else{
  
                  init_op_code(obj,op);
                  if (is_format_two(op)) { // format 2 object code
                      obj->format = 2;
  
                      size_t reg1 = find_register_no(operand1);
                      size_t reg2 = find_register_no(operand2);
  
                      if (reg1 == -1) {
                      #ifdef DEBUG
                          printf("\033[31m Register for operand1 not found \033[0m\n");
                      #endif
                          init_registers(obj, 0);  // Default to register 0
                      } else {
                          init_registers(obj, reg1);
                      }
  
                      if (reg2 == -1) {
                      #ifdef DEBUG
                          printf("\033[31m Register for operand2 not found \033[0m\n");
                      #endif
                          init_registers(obj, 0);  // Default to register 0
                      } else {
                          init_registers(obj, reg2);
                      }
                  }
                  else{ // format 3 object code
                      size_t addr_mode;
                      size_t registers;
  
                      char first = operand1[0];
                      if (first == '#') {
                          addr_mode = 1;  // Immediate addressing
                      } else if (first == '@') {
                          addr_mode = 2;  // Indirect addressing
                      } else {
                          addr_mode = 3;  // Simple addressing
                      }
  
                      init_address_mode(obj, addr_mode);
  
                      // Set registers
                      if (abs(findSymbol(operand1) - pg_loc - 3) > 4096) {
                          registers = 4;
                          base_relative = 1;
                      } else {
                          registers = 2;
                      }
  
                      if (OPERAND_NOT_EMPTY(operand2)) {
                          registers += 8;
                      } else if (obj->format == 4) {
                          registers = 1;
                      } else if (IS_INTEGER_WITH_HASH(operand1) || !OPERAND_NOT_EMPTY(operand1)) {
                          registers = 0;
                      }
  
                  #ifdef PASS_TWO_DEBUG
                      printf("operand1 = %s\n", operand1);
                  #endif
  
                  init_registers(obj, registers);
              }
              switch(obj->format){
                  case 2:{
                      break;
                  }
                  case 3:
                  {
                      int s_v = findSymbol(operand1);
                      // printf("s_v [%d] pg_loc [%d] disp [%03X]\n",s_v,pg_loc,s_v - pg_loc);
                      if (s_v){
                      if(base_relative)
                          init_disp(obj, s_v - base);
                      else
                          init_disp(obj, s_v - pg_loc - 3);
                      }
                      else if (operand1[0] == '#' && isInteger(operand1+1)){
                      size_t immediate_addr = strtol(operand1+1, NULL, 10);
                      init_disp(obj, immediate_addr);
                      }
                      else if (operand1[0] == '\0')
                      init_disp(obj, 0);
                      #ifdef DEBUG
                      else
                      printf("\033[31m Symbol not found\033[0m\n");
                      #endif
                      break;
                  }
                  case 4:
                  {
                      int s_v = findSymbol(operand1);
                      // printf("s_v [%d] pg_loc [%d] disp [%03X]\n",s_v,pg_loc,s_v - pg_loc);
                      if(!IS_INTEGER_WITH_HASH(operand1))
                          addMRecord(pg_loc, 5);
                      if (s_v)
                          init_disp4(obj, s_v);
                      else if (operand1[0] == '#' && isInteger(operand1 + 1))
                      {
                          size_t immediate_addr = strtol(operand1 + 1, NULL, 10);
                          init_disp4(obj, immediate_addr);
                      }
                      else if (operand1[0] == '\0')
                          init_disp4(obj, 0);
                      else
                          perror("\033[31m Symbol not found\033[0m\n");
                  }
                  default:
                      break;
              }
                  
              }
              // print_obj_code(obj);
              insert_object_list(obj);
              }
              else if(CMP(op, D_RESW)){
                  obj = createObjectCode(pg_loc);
                  INIT_OBJECT_CODE(obj);
                  insert_object_list(obj);
              }
              write_line(fd, obj);
          }
      }
  
      close(fd);
  }

```

操作步驟

```
make
```
```
./asm input.asm
```
input.asm 為輸入檔案

產生 asm.o 執行檔 內容為機器碼，機器碼的組成由Header Record、Text Record及End Record組成。
out.obj  
Header Record的第一個字為H，第一個label個字為程式名稱，程式的起始位址，程式長度(計算出來的)
```c
    printf("H%s  %06X%06X\n", program_name, start_loc, pg_loc - start_loc);
```
Text Record共分成三個部分，第一個部分為初始化Text Record，即 T + Text Record的初始位址，之後接著兩碼為這段程式的長度，但由於現在不知道是多少，只能暫時空著。 
add_text中，檢查如果程式的長度超過一行Text Record所能乘載的字數(先預設為0x1D)，就把這行寫上，並初始化一條新的。 
```c
  struct ObjectCode *o_ptr = ObjectCodeList.head;
    while (o_ptr) {
        unsigned int hex_count = 0;
        struct ObjectCode *o_seek = o_ptr;
        while (hex_count < 0x1D && o_seek) {
            if (o_seek->format == 0) {
                while (o_seek->format==0 && o_seek->next)
                {
                    o_seek = o_seek->next;
                }
                break;
            }
            hex_count += o_seek->format;
            o_seek = o_seek->next;
        }
        printf("T%06X%02X", o_ptr->address, hex_count);
        while (o_ptr != o_seek) {
            print_code(o_ptr);
            o_ptr = o_ptr->next;
            if (!o_ptr) {
                break;
            }
        }
        printf("\n");
    }

```
out.log 紀錄檔 symbol table
