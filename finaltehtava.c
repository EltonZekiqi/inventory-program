#include <stdio.h>

#define MAX_PRODUCTS 500
#define MAX_QUANTITY 100000
#define DATA_FILENAME "inventory.txt"

// Funktiot
void ask_product(int table[]);
void print_products(int table[]);
void read_items(int table[], const char *filename);
void write_items(int table[], const char *filename);

int main(void) {
    int table[MAX_PRODUCTS + 1] = {0};
    char choice;

    // yritetään ladata vanha inventory
    read_items(table, DATA_FILENAME);

    printf("Inventory program (products 1..%d)\n", MAX_PRODUCTS);

    do {
        ask_product(table);

        printf("Add more? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    printf("\nHere’s what you entered:\n");
    print_products(table);

    printf("\nSave to file? (y/n): ");
    scanf(" %c", &choice);
    if (choice == 'y' || choice == 'Y') {
        write_items(table, DATA_FILENAME);
        printf("Saved! Bye!\n");
    } else {
        printf("Not saved. Bye!\n");
    }

    return 0;
}

// Kysytään tuote ja määrä
void ask_product(int table[]) {
    int prod_id, amount;
    printf("\nProduct number (1-%d): ", MAX_PRODUCTS);
    scanf("%d", &prod_id);

    if (prod_id < 1) prod_id = 1;
    if (prod_id > MAX_PRODUCTS) prod_id = MAX_PRODUCTS;

    printf("Amount for product %d (0-%d): ", prod_id, MAX_QUANTITY);
    scanf("%d", &amount);
    if (amount < 0) amount = 0;
    if (amount > MAX_QUANTITY) amount = MAX_QUANTITY;

    table[prod_id] = amount;
    printf("Saved: product %d = %d pcs\n", prod_id, amount);
}

// Printtaa kaikki tuotteet >0
void print_products(int table[]) {
    for (int i = 1; i <= MAX_PRODUCTS; i++) {
        if (table[i] > 0) {
            printf("product %d: %d pcs\n", i, table[i]);
        }
    }
}

// Lataa tiedostosta
void read_items(int table[], const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("No previous inventory, starting fresh.\n");
        return;
    }

    for (int i = 1; i <= MAX_PRODUCTS; i++) {
        if (fscanf(fp, "%d", &table[i]) != 1) table[i] = 0;
    }
    fclose(fp);
}

// Tallentaa tiedostoon
void write_items(int table[], const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Cannot write file!\n");
        return;
    }

    for (int i = 1; i <= MAX_PRODUCTS; i++) {
        fprintf(fp, "%d ", table[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}
