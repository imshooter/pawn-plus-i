#include <open.mp>

#define PP_SYNTAX

#if !defined PP_ADDITIONAL_TAGS
    #define PP_ADDITIONAL_TAGS Item,ItemBuild,Container
#endif

#include <II\item>
#include <II\container>

main(){}

public OnGameModeInit() {
    new const Container:containerid = CreateContainer("Any Container", 100000);
    new count = 0, index = 0, t = 0, Item:searchItem;
    
    /**
     * CreateItem
     *  - Iter_Alloc: ~4417 ms
     *  - pool_add: ~80 ms
     */
    new const t3 = GetTickCount();
    for (new i; i != 100000; i++) {
        CreateItem(ItemBuild:(i % 10), .call = false);
        count++;
    }
    printf("Create Item: %i ms (count: %i)", GetTickCount() - t3, count);

    count = 0;
    t = GetTickCount();

    for (new i; i != 100000; i++) {
        if (!AddItemToContainer(containerid, Item:i, index, .call = false)) {
            continue;
        }
        count++;
    }
    printf("Container Add Item: %i ms (count: %i, last index: %i)", GetTickCount() - t, count, index);
    
    count = 0;
    t = GetTickCount();

    for (new i; i != 100000; i++) {
        if (!RemoveItemFromContainer(containerid, i, searchItem, .call = false)) {
            continue;
        }
        count++;
    }
    printf("Container Remove Item: %i ms (count: %i, last item: %i)", GetTickCount() - t, count, _:searchItem);

    // Add Item Back to Container
    for (new i; i != 100000; i++) {
        if (!AddItemToContainer(containerid, Item:i, index, .call = false)) {
            continue;
        }
        count++;
    }

    count = 0;
    t = GetTickCount();

    for (new Iter:iter = GetContainerIter(containerid), Item:itemid; iter_inside(iter); iter_move_next(iter)) {
        if (!iter_get_safe(iter, itemid)) {
            continue;
        }
        if (IsValidItem(itemid)) {
            count++;
        }
    }
    printf("Container Iterations: %i ms (count: %i)", GetTickCount() - t, count);

    return 1;
}