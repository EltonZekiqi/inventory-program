#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 500
#define MAX_QUANTITY 100000
#define DATA_FILENAME "inventory.txt"

// Värien määrittelyt (Color pairs)
#define CP_NORMAL 1
#define CP_HEADER 2
#define CP_INPUT  3
#define CP_ERROR  4

// Globaali taulukko (yksinkertaisuuden vuoksi tässä esimerkissä)
int table[MAX_PRODUCTS + 1] = {0};

// Funktioiden esittelyt
void init_ncurses();
void close_ncurses();
void draw_header(const char *title);
void read_items(const char *filename);
void write_items(const char *filename);

// Näkymä-funktiot (Windows/Views)
void view_menu();
void view_add_product();
void view_list_products();
void view_help();

int main(void) {
    // 1. Ladataan tiedot heti alussa
    read_items(DATA_FILENAME);

    // 2. Käynnistetään ncurses
    init_ncurses();

    // 3. Päävalikko-looppi on siirretty view_menu-funktioon
    view_menu();

    // 4. Lopetus
    close_ncurses();
    return 0;
}

// --- NCURSES ALUSTUS JA LOPETUS ---

void init_ncurses() {
    initscr();            // Aloita ncurses
    cbreak();             // Ei puskurointia rivinvaihtoon asti
    noecho();             // Älä tulosta näppäimiä automaattisesti
    keypad(stdscr, TRUE); // Salli nuolinäppäimet
    curs_set(0);          // Piilota kursori

    if (has_colors()) {
        start_color();
        // Määritellään väripareja: (ID, Teksti, Tausta)
        init_pair(CP_NORMAL, COLOR_WHITE, COLOR_BLUE);  // Perusväri
        init_pair(CP_HEADER, COLOR_BLACK, COLOR_WHITE); // Otsikot
        init_pair(CP_INPUT,  COLOR_BLACK, COLOR_CYAN);  // Syöttökentät
        init_pair(CP_ERROR,  COLOR_RED,   COLOR_BLACK); // Virheet
    }
    
    // Asetetaan koko ruudun taustaväri
    bkgd(COLOR_PAIR(CP_NORMAL));
}

void close_ncurses() {
    endwin();
}

// --- APUFUNKTIOT ---

// Piirtää yläpalkin ja alapalkin
void draw_header(const char *title) {
    clear();
    
    // Yläpalkki
    attron(COLOR_PAIR(CP_HEADER));
    mvprintw(0, 0, "%-80s", " INVENTORY MANAGEMENT SYSTEM v2.0"); 
    attroff(COLOR_PAIR(CP_HEADER));

    // Ikkunan otsikko
    attron(A_BOLD);
    mvprintw(2, 2, ">> %s", title);
    attroff(A_BOLD);

    // Piirretään laatikko (reunat)
    box(stdscr, 0, 0);

    // Alapalkki ohjeille
    mvprintw(LINES - 2, 2, "Press key to navigate...");
}

// Turvallinen tapa kysyä kokonaislukua ncursesilla
int get_int_input(int y, int x) {
    char str[10];
    echo();             // Näytetään kirjoitus
    curs_set(1);        // Näytetään kursori
    attron(COLOR_PAIR(CP_INPUT));
    mvprintw(y, x, "        "); // Tyhjennetään kenttä
    mvgetnstr(y, x, str, 8);    // Luetaan max 8 merkkiä
    attroff(COLOR_PAIR(CP_INPUT));
    noecho();
    curs_set(0);
    return atoi(str);   // Muutetaan intiksi (palauttaa 0 jos virhe)
}

// --- NÄKYMÄT (VIEWS / WINDOWS) ---

void view_menu() {
    int choice;
    int highlight = 0;
    const char *options[] = {
        "1. Add / Edit Product",
        "2. List Inventory",
        "3. Help",
        "4. Save & Exit",
        "5. Exit without Saving"
    };
    int n_options = 5;

    while(1) {
        draw_header("MAIN MENU");

        // Piirretään menu
        for(int i = 0; i < n_options; i++) {
            if(i == highlight) attron(A_REVERSE); // Korostus
            mvprintw(5 + i, 5, "%s", options[i]);
            if(i == highlight) attroff(A_REVERSE);
        }

        mvprintw(LINES - 2, 2, "Use UP/DOWN arrows to select, ENTER to choose.");

        choice = getch();

        switch(choice) {
            case KEY_UP:
                highlight--;
                if(highlight < 0) highlight = n_options - 1;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight >= n_options) highlight = 0;
                break;
            case 10: // Enter-näppäin
                if(highlight == 0) view_add_product();
                if(highlight == 1) view_list_products();
                if(highlight == 2) view_help();
                if(highlight == 3) {
                    write_items(DATA_FILENAME);
                    return; // Lopettaa ohjelman
                }
                if(highlight == 4) return; // Lopettaa tallentamatta
                break;
        }
    }
}

void view_add_product() {
    int id = 0, amount = 0;
    char confirm;

    while(1) {
        draw_header("ADD / EDIT PRODUCT");
        
        // Piirretään "lomake" (Form)
        mvprintw(5, 5, "Enter Product details below:");
        
        // Field 1: ID
        mvprintw(7, 5, "Product ID (1-%d):", MAX_PRODUCTS);
        attron(COLOR_PAIR(CP_INPUT));
        mvprintw(7, 30, "[      ]");
        attroff(COLOR_PAIR(CP_INPUT));

        // Field 2: Amount
        mvprintw(9, 5, "Quantity (0-%d):", MAX_QUANTITY);
        attron(COLOR_PAIR(CP_INPUT));
        mvprintw(9, 30, "[      ]");
        attroff(COLOR_PAIR(CP_INPUT));

        mvprintw(LINES - 2, 2, "Type ID and press ENTER.");

        // Kysytään ID
        id = get_int_input(7, 31);

        // Validointi
        if (id < 1 || id > MAX_PRODUCTS) {
            attron(COLOR_PAIR(CP_ERROR));
            mvprintw(12, 5, "ERROR: Invalid Product ID!");
            attroff(COLOR_PAIR(CP_ERROR));
            getch();
            continue; // Uusiksi
        }

        // Näytetään nykyinen arvo, jos on
        mvprintw(8, 30, "Current: %d", table[id]);

        // Kysytään määrä
        mvprintw(LINES - 2, 2, "Type Amount and press ENTER.");
        amount = get_int_input(9, 31);

        if (amount < 0 || amount > MAX_QUANTITY) {
            attron(COLOR_PAIR(CP_ERROR));
            mvprintw(12, 5, "ERROR: Invalid Amount!");
            attroff(COLOR_PAIR(CP_ERROR));
            getch();
            continue;
        }

        // Tallennus muistiin
        table[id] = amount;
        
        attron(A_BLINK);
        mvprintw(12, 5, "SAVED: Product %d is now %d pcs.", id, amount);
        attroff(A_BLINK);

        mvprintw(14, 5, "Add another? (y/n)");
        confirm = getch();
        if(confirm != 'y' && confirm != 'Y') break;
    }
}

void view_list_products() {
    draw_header("INVENTORY LIST");
    
    int row = 5;
    int count = 0;

    mvprintw(4, 5, "%-10s | %-10s", "PROD ID", "QUANTITY");
    mvprintw(5, 5, "-----------+----------");
    row++;

    for (int i = 1; i <= MAX_PRODUCTS; i++) {
        if (table[i] > 0) {
            // Jos lista on pitkä, pitäisi tehdä sivutus (paging).
            // Tässä yksinkertainen versio joka pysähtyy jos ruutu täynnä.
            if(row >= LINES - 3) {
                mvprintw(row, 5, "... (more items hidden, press key) ...");
                break; 
            }
            mvprintw(row, 5, "Product %-2d | %d pcs", i, table[i]);
            row++;
            count++;
        }
    }

    if(count == 0) {
        mvprintw(7, 5, "Inventory is empty.");
    }

    mvprintw(LINES - 2, 2, "Press any key to return to menu...");
    getch();
}

void view_help() {
    draw_header("HELP / INSTRUCTIONS");

    mvprintw(5, 5, "How to use:");
    mvprintw(7, 5, "- Use ARROW KEYS to navigate the menu.");
    mvprintw(8, 5, "- ENTER selects an option.");
    mvprintw(9, 5, "- In 'Add Product', type numbers and press ENTER.");
    mvprintw(11, 5, "Saving:");
    mvprintw(12, 5, "- Data is saved to '%s' only when", DATA_FILENAME);
    mvprintw(13, 5, "  you choose 'Save & Exit'.");
    
    attron(COLOR_PAIR(CP_ERROR));
    mvprintw(15, 5, "Note: Maximum product ID is %d.", MAX_PRODUCTS);
    attroff(COLOR_PAIR(CP_ERROR));

    mvprintw(LINES - 2, 2, "Press any key to return...");
    getch();
}

// --- TIEDOSTO LOGIIKKA (Sama kuin alkuperäisessä) ---

void read_items(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return; // Aloitetaan tyhjästä

    for (int i = 1; i <= MAX_PRODUCTS; i++) {
        if (fscanf(fp, "%d", &table[i]) != 1) table[i] = 0;
    }
    fclose(fp);
}

void write_items(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        // Ncurses-tilassa ei voi käyttää printf, tässä virhettä ei näytetä
        // ellei tehdä erillistä virheilmoitusikkunaa.
        return;
    }

    for (int i = 1; i <= MAX_PRODUCTS; i++) {
        fprintf(fp, "%d ", table[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}
