typedef struct Item Item;

typedef struct Data {
	char * ssid;
	char * password;
} Data;

typedef struct Item {
	Data * data;
	Item * next;
} Item;

typedef struct Head {
	Item * head;
} Head;



Head * list_init(void);
void list_add(Head * head, char * ssid, char * password);
void list_print(Head * head);
