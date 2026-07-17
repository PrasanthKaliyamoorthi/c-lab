# Log Analyzer Engine — Architecture Design

## Overview

```
┌─────────────────────────────────────────────────────────────┐
│                   LOG ANALYZER ENGINE                        │
│                    Architecture                              │
└─────────────────────────────────────────────────────────────┘

┌──────────┐   stdin    ┌──────────────────────────────────────┐
│ sample   │──────────▶│           main()                      │
│ .log3    │           │  - reads lines via getline            │
└──────────┘           │  - dispatches to parse_line           │
                       └────────────┬─────────────────────────┘
                                    │
                    ┌───────────────┼───────────────┐
                    ▼               ▼               ▼
              ┌──────────┐   ┌──────────┐   ┌──────────────┐
              │  QUEUE   │   │  SMTP    │   │  DELIVERY    │
              │  event   │   │  event   │   │  event       │
              └────┬─────┘   └────┬─────┘   └──────┬───────┘
                   │              │                │
                   ▼              ▼                ▼
           ┌──────────────────────────────────────────────┐
           │           Correlation Layer                   │
           │                                               │
           │  find_by_queue_id() ◄── SMTP matches by qid   │
           │  find_by_span_id()  ◄── DELIVERY by span      │
           │  init_transaction() ◄── QUEUE creates txn     │
           └──────────────────────┬───────────────────────┘
                                  │
                                  ▼
           ┌──────────────────────────────────────────────┐
           │           Data Structures                     │
           │                                               │
           │  TransactionList                              │
           │  ┌──────────────────────────────────────┐     │
           │  │ Transaction[0] { qid, span, sender,  │     │
           │  │   recipients[], status, first_seen } │     │
           │  │ Transaction[1] { ... }               │     │
           │  │ Transaction[2] { ... }               │     │
           │  └──────────────────────────────────────┘     │
           │  .count  .capacity                            │
           └──────────────────────┬───────────────────────┘
                                  │
                                  ▼
           ┌──────────────────────────────────────────────┐
           │           Output Layer                        │
           │                                               │
           │  sort_by_timestamp()  ── insertion sort       │
           │  print_report()       ── prints per-txn +     │
           │                          statistics           │
           └──────────────────────────────────────────────┘
```

## Data Flow

```
1. QUEUE id=Q100 span=S1 from=x
   → create Transaction{ qid=Q100, span=S1, sender=x }

2. SMTP id=Q100 rcpt=a@test.com
   → find Transaction by qid=Q100 → add recipient "a"

3. DELIVERY span=S1 status=success
   → find Transaction by span=S1 → set status="success"

4. Unknown qid/span → print WARNING
```

## Function Map

| Function | Purpose |
|---|---|
| `parse_timestamp()` | Convert ISO 8601 string to Unix epoch |
| `tokenize()` | Split string by delimiter into array |
| `free_tokens()` | Free dynamically allocated token array |
| `find_by_queue_id()` | Linear search TransactionList by queue_id |
| `find_by_span_id()` | Linear search TransactionList by span_id |
| `ensure_capacity()` | Grow TransactionList dynamic array (double) |
| `add_recipient()` | Grow recipients dynamic array + strdup |
| `init_transaction()` | Populate new Transaction from QUEUE event |
| `free_transaction()` | Free recipients array for one Transaction |
| `sort_by_timestamp()` | Insertion sort by first_seen |
| `print_report()` | Print per-transaction report + statistics |

## Memory Lifecycle

```
Allocation:
  main()     → malloc(TransactionList.items)
  tokenize() → calloc(tokens) + strdup(each field)
  add_recipient → realloc(recipients) + strdup(rcpt)

Deallocation:
  free_tokens()   → frees strdup'd tokens + array
  free_transaction → frees strdup'd recipients + array
  main() cleanup → frees TransactionList.items + line
```

## Key Design Decisions

- **Dynamic arrays** instead of linked lists for predictable memory layout
- **Linear search** for correlation (Extra Hard mode would replace with hash table)
- **Insertion sort** to satisfy "no qsort" constraint (Bonus 3)
- **strdup** for string ownership — every string has a clear free site
- **stdin** input for pipe-ability, composable with other tools
