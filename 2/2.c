#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// hashmap implementation from:
// https://github.com/engineer-man/youtube/blob/master/077/hashtable.c
// my code starts at line 224
#define TABLE_SIZE 20000

typedef struct entry_t {
    char *key;
    char *value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries;
} ht_t;

unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // do several rounds of multiplication
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // make sure value is 0 <= value < TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}

entry_t *ht_pair(const char *key, const char *value) {
    // allocate the entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    // copy the key and value in place
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next starts out null but may be set later on
    entry->next = NULL;

    return entry;
}

ht_t *ht_create(void) {
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    // set each to null (needed for proper operation)
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t *hashtable, const char *key, const char *value) {
    unsigned int slot = hash(key);

    // try to look up an entry set
    entry_t *entry = hashtable->entries[slot];

    // no entry means slot empty, insert immediately
    if (entry == NULL) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;

    // walk through each entry until either the end is
    // reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // match found, replace value
            free(entry->value);
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = ht_pair(key, value);
}

char *ht_get(ht_t *hashtable, const char *key) {
    unsigned int slot = hash(key);

    // try to find a valid slot
    entry_t *entry = hashtable->entries[slot];

    // no slot means no entry
    if (entry == NULL) {
        return NULL;
    }

    // walk through each entry in the slot, which could just be a single thing
    while (entry != NULL) {
        // return value if found
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        // proceed to next key if available
        entry = entry->next;
    }

    // reaching here means there were >= 1 entries but no key match
    return NULL;
}

void ht_del(ht_t *hashtable, const char *key) {
    unsigned int bucket = hash(key);

    // try to find a valid bucket
    entry_t *entry = hashtable->entries[bucket];

    // no bucket means no entry
    if (entry == NULL) {
        return;
    }

    entry_t *prev;
    int idx = 0;

    // walk through each entry until either the end is reached or a matching key is found
    while (entry != NULL) {
        // check key
        if (strcmp(entry->key, key) == 0) {
            // first item and no next entry
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            // first item with a next entry
            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = entry->next;
            }

            // last item
            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            // middle item
            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            // free the deleted entry
            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        // walk to next
        prev = entry;
        entry = prev->next;

        ++idx;
    }
}

void ht_dump(ht_t *hashtable) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t *entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for (;;) {
            printf("%s=%s ", entry->key, entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

// int main(int argc, char **argv) {
//     ht_t *ht = ht_create();
//
//     ht_set(ht, "name1", "em");
//     ht_set(ht, "name2", "russian");
//     ht_set(ht, "name3", "pizza");
//     ht_set(ht, "name4", "doge");
//     ht_set(ht, "name5", "pyro");
//     ht_set(ht, "name6", "joost");
//     ht_set(ht, "name7", "kalix");
//
//     ht_dump(ht);
//
//     return 0;
// }

int main() {
    char buf[1024];
    int c;
    char *s = buf;
    while ((c = fgetc(stdin)) != EOF && s < buf + sizeof buf - 1) {
        if (c == '\n' && s > buf && s[-1] == '\n') {
            ungetc(c, stdin);
            break;
        }
        *s++ = c;
    }
    *s = '\0';

    // {"123": "Keelvar"}
    ht_t *ht_country = ht_create();

    // count occurence
    // {"123": "2"}
    ht_t *ht_count = ht_create();

    const char outer_delimiters[] = "\n";
    const char inner_delimiters[] = " ";

    char *outer_token;
    char *inner_token;
    char *outer_saveptr = NULL;
    char *inner_saveptr = NULL;

    outer_token = strtok_r(buf, outer_delimiters, &outer_saveptr);
    int country_len = atoi(outer_token);
    char country_buf[1024] = "";
    char *id_arr[100];

    for (int i = 0; i < country_len; i++) {
        // steps for appending string with newline, using hashtable instead
        // outer_token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
        // pos += snprintf(&country_buf[pos], "%s", outer_token);
        // pos += snprintf(&country_buf[pos], "%s", "\n");

        outer_token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
        char *country = strtok_r(outer_token, inner_delimiters, &inner_saveptr);
        char *id = strtok_r(NULL, inner_delimiters, &inner_saveptr);
        ht_set(ht_country, id, country);

        // build an array for loop later
        id_arr[i] = id;
    }
    // ht_dump(ht_country);

    char *dimension = strtok_r(NULL, outer_delimiters, &outer_saveptr);
    int row = atoi(strtok_r(dimension, inner_delimiters, &inner_saveptr));
    int col = atoi(strtok_r(NULL, inner_delimiters, &inner_saveptr));

    for (int i = 0; i < row; i++) {
        outer_token = strtok_r(NULL, outer_delimiters, &outer_saveptr);

        for (int j = 0; j < col; j++) {
            if (j == 0) {
                inner_token = strtok_r(outer_token, inner_delimiters, &inner_saveptr);
            } else {
                inner_token = strtok_r(NULL, inner_delimiters, &inner_saveptr);
            }

            int value = ht_get(ht_count, inner_token);
            if (value == NULL) {
                // hashtable only takes in string
                ht_set(ht_count, inner_token, "1");
            } else {
                char str[100];
                int newValue = atoi(ht_get(ht_count, (inner_token))) + 1;
                sprintf(str, "%d", newValue);
                ht_set(ht_count, inner_token, str);
            }
        }
    }

    char *max_value_id;
    int max_value = 0;

    // no hashtable sorting function:(
    // loops 3 times since only print top 3 occurence
    // remove record from hashtable once max is found for each round
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < country_len; j++) {
            int value = 0;
            if (ht_get(ht_count, id_arr[j]) != NULL) {
                value = atoi(ht_get(ht_count, id_arr[j]));
            }
            if (value > max_value) {
                max_value = value;
                max_value_id = id_arr[j];
            }
        }
        printf("%s\n", ht_get(ht_country, max_value_id));

        // instead of deleting, set it to zero to avoid seg fault
        // ht_del(ht_count, max_value_id);
        ht_set(ht_count, max_value_id, "0");
        max_value = 0;
    }
    return 0;
}
