#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LEN 51
#define INITIAL_CAPACITY 4
#define MAX_INITIAL 100

typedef struct {
    int id;
    char name[NAME_LEN];
    float price;
    int quantity;
} Product;

typedef enum {
    MENU_ADD = 1,
    MENU_VIEW,
    MENU_UPDATE_QTY,
    MENU_SEARCH_ID,
    MENU_SEARCH_NAME,
    MENU_SEARCH_PRICE,
    MENU_DELETE,
    MENU_EXIT
} MenuOption;

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

static void to_lower_copy(const char *src, char *dest, size_t dest_size) {
    size_t i = 0;
    while (*src != '\0' && i + 1 < dest_size) {
        dest[i++] = (char)tolower((unsigned char)*src++);
    }
    dest[i] = '\0';
}

static int find_index_by_id(const Product *arr, size_t count, int id) {
    for (size_t i = 0; i < count; ++i) {
        if (arr[i].id == id) {
            return (int)i;
        }
    }
    return -1;
}

static int ensure_capacity(Product **arr_ptr, size_t *capacity_ptr, size_t min_capacity) {
    if (*capacity_ptr >= min_capacity) {
        return 1;
    }
    size_t new_capacity = (*capacity_ptr == 0) ? INITIAL_CAPACITY : *capacity_ptr;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    Product *tmp = realloc(*arr_ptr, new_capacity * sizeof(Product));
    if (tmp == NULL) {
        return 0;
    }
    *arr_ptr = tmp;
    *capacity_ptr = new_capacity;
    return 1;
}

static void input_initial_products(Product *arr, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        printf("Enter details for product %zu:\n", i + 1);

        printf("Product ID: ");
        while (scanf("%d", &arr[i].id) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Enter an integer for Product ID: ");
        }
        while (getchar() != '\n');

        printf("Product Name: ");
        read_line(arr[i].name, NAME_LEN);

        printf("Product Price: ");
        while (scanf("%f", &arr[i].price) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Enter a number for Price: ");
        }

        printf("Product Quantity: ");
        while (scanf("%d", &arr[i].quantity) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Enter an integer for Quantity: ");
        }
        while (getchar() != '\n');
    }
}

static void view_all_products(const Product *arr, size_t count) {
    if (count == 0) {
        printf("No products in inventory.\n");
        return;
    }
    printf("\n========= PRODUCT LIST =========\n");
    for (size_t i = 0; i < count; ++i) {
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               arr[i].id, arr[i].name, arr[i].price, arr[i].quantity);
    }
}

static void add_product(Product **arr_ptr, size_t *count_ptr, size_t *capacity_ptr) {
    Product new_prod;
    printf("Enter new product details:\n");

    printf("Product ID: ");
    if (scanf("%d", &new_prod.id) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    if (find_index_by_id(*arr_ptr, *count_ptr, new_prod.id) != -1) {
        printf("Product ID already exists. Aborting add.\n");
        return;
    }

    printf("Product Name: ");
    read_line(new_prod.name, NAME_LEN);

    printf("Product Price: ");
    if (scanf("%f", &new_prod.price) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }

    printf("Product Quantity: ");
    if (scanf("%d", &new_prod.quantity) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    size_t needed = *count_ptr + 1;
    if (!ensure_capacity(arr_ptr, capacity_ptr, needed)) {
        printf("Memory allocation failed. Cannot add product.\n");
        return;
    }

    (*arr_ptr)[*count_ptr] = new_prod;
    *count_ptr = *count_ptr + 1;
    printf("Product added successfully!\n");
}

static void update_quantity(Product *arr, size_t count) {
    int id;
    printf("Enter Product ID to update quantity: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    int idx = find_index_by_id(arr, count, id);
    if (idx == -1) {
        printf("Product ID %d not found.\n", id);
        return;
    }

    int new_qty;
    printf("Enter new Quantity: ");
    if (scanf("%d", &new_qty) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    arr[idx].quantity = new_qty;
    printf("Quantity updated successfully!\n");
}

static void search_by_id(const Product *arr, size_t count) {
    int id;
    printf("Enter Product ID to search: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    int idx = find_index_by_id(arr, count, id);
    if (idx == -1) {
        printf("Product with ID %d not found.\n", id);
        return;
    }
    printf("Product Found: Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           arr[idx].id, arr[idx].name, arr[idx].price, arr[idx].quantity);
}

static void search_by_name(const Product *arr, size_t count) {
    char query[NAME_LEN];
    printf("Enter name to search (partial allowed): ");
    read_line(query, NAME_LEN);
    if (query[0] == '\0') {
        printf("Empty search string.\n");
        return;
    }

    char qlower[NAME_LEN];
    to_lower_copy(query, qlower, NAME_LEN);

    int found = 0;
    for (size_t i = 0; i < count; ++i) {
        char name_lower[NAME_LEN];
        to_lower_copy(arr[i].name, name_lower, NAME_LEN);
        if (strstr(name_lower, qlower) != NULL) {
            if (!found) {
                printf("Products Found:\n");
            }
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   arr[i].id, arr[i].name, arr[i].price, arr[i].quantity);
            found = 1;
        }
    }
    if (!found) {
        printf("No products matched the name \"%s\".\n", query);
    }
}

static void search_by_price_range(const Product *arr, size_t count) {
    float low, high;
    printf("Enter minimum price: ");
    if (scanf("%f", &low) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    printf("Enter maximum price: ");
    if (scanf("%f", &high) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    if (low > high) {
        printf("Invalid range: min > max.\n");
        return;
    }

    int found = 0;
    for (size_t i = 0; i < count; ++i) {
        if (arr[i].price >= low && arr[i].price <= high) {
            if (!found) {
                printf("Products in price range:\n");
            }
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   arr[i].id, arr[i].name, arr[i].price, arr[i].quantity);
            found = 1;
        }
    }
    if (!found) {
        printf("No products found in the given price range.\n");
    }
}

static void delete_product(Product **arr_ptr, size_t *count_ptr, size_t *capacity_ptr) {
    Product *arr = *arr_ptr;
    int id;
    printf("Enter Product ID to delete: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n');

    int idx = find_index_by_id(arr, *count_ptr, id);
    if (idx == -1) {
        printf("Product ID %d not found.\n", id);
        return;
    }

    for (size_t i = (size_t)idx; i + 1 < *count_ptr; ++i) {
        arr[i] = arr[i + 1];
    }
    *count_ptr = *count_ptr - 1;

    if (*count_ptr == 0) {
        free(arr);
        *arr_ptr = NULL;
        *capacity_ptr = 0;
        printf("Product deleted. Inventory is now empty.\n");
        return;
    }

    /* shrink capacity if too large (optional): reduce when size <= capacity/4 */
    if (*capacity_ptr > INITIAL_CAPACITY && *count_ptr <= (*capacity_ptr / 4)) {
        size_t new_capacity = *capacity_ptr / 2;
        if (new_capacity < INITIAL_CAPACITY) {
            new_capacity = INITIAL_CAPACITY;
        }
        Product *tmp = realloc(arr, new_capacity * sizeof(Product));
        if (tmp != NULL) {
            *arr_ptr = tmp;
            *capacity_ptr = new_capacity;
        }
    }
    printf("Product deleted successfully!\n");
}

int main(void) {
    int initial = 0;
    printf("Enter initial number of products: ");
    if (scanf("%d", &initial) != 1) {
        printf("Invalid input.\n");
        return 1;
    }
    while (getchar() != '\n');

    if (initial < 1 || initial > MAX_INITIAL) {
        printf("Invalid initial count. Must be between 1 and %d.\n", MAX_INITIAL);
        return 1;
    }

    size_t product_count = (size_t)initial;
    size_t capacity = (size_t)initial;
    if (capacity < INITIAL_CAPACITY) {
        capacity = INITIAL_CAPACITY;
    }

    Product *products = calloc(capacity, sizeof(Product));
    if (products == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    input_initial_products(products, product_count);

    int choice = 0;
    for (;;) {
        printf("\n========= INVENTORY MENU =========\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid choice.\n");
            continue;
        }
        while (getchar() != '\n');

        MenuOption opt = (MenuOption)choice;
        switch (opt) {
            case MENU_ADD:
                add_product(&products, &product_count, &capacity);
                break;
            case MENU_VIEW:
                view_all_products(products, product_count);
                break;
            case MENU_UPDATE_QTY:
                update_quantity(products, product_count);
                break;
            case MENU_SEARCH_ID:
                search_by_id(products, product_count);
                break;
            case MENU_SEARCH_NAME:
                search_by_name(products, product_count);
                break;
            case MENU_SEARCH_PRICE:
                search_by_price_range(products, product_count);
                break;
            case MENU_DELETE:
                delete_product(&products, &product_count, &capacity);
                break;
            case MENU_EXIT:
                free(products);
                printf("Memory released. Exiting program.\n");
                return 0;
            default:
                printf("Please enter a valid choice (1-8).\n");
        }
    }
}