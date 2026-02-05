#if defined _container_included
    #endinput
#endif
#define _container_included

#if defined _item_included
    #include <II\item>
#endif

#include <pp-hooks>

#define MAX_CONTAINERS (Container:2048)
#define MAX_CONTAINER_NAME (16)
#define INVALID_CONTAINER_ID (Container:-1)

static Pool:containerItemCollection[MAX_CONTAINERS] = { INVALID_POOL, ... };
static Container:itemContainerID[MAX_ITEMS] = { INVALID_CONTAINER_ID, ... };
static itemContainerIndex[MAX_ITEMS] = { -1, ... };

// Global Iterator
static Pool:allContainersCollection;

/**
 * @Functions
 */

forward Container:CreateContainer(const name[], capacity, bool:call = true);
forward bool:IsValidContainer(Container:containerid);
forward bool:SetContainerName(Container:containerid, const name[]);
forward bool:GetContainerName(Container:containerid, output[], size = sizeof (output));
forward bool:AddItemToContainer(Container:containerid, Item:itemid, &index = -1, addToIndex = -1, playerid = INVALID_PLAYER_ID, bool:call = true);
forward bool:RemoveItemFromContainer(Container:containerid, index, &Item:itemid = INVALID_ITEM_ID, playerid = INVALID_PLAYER_ID, bool:call = true);
forward bool:IsContainerSlotUsed(Container:containerid, index);
forward Item:GetContainerSlotItem(Container:containerid, index);
forward Iter:GetContainerIter(Container:containerid, index = 0);
forward Pool:GetContainerPool();

/**
 * @External
 */

stock Container:CreateContainer(const name[], capacity, bool:call = true) {
    if (pool_size(allContainersCollection) >= _:MAX_CONTAINERS) {
        return INVALID_CONTAINER_ID;
    }

    new const Container:index = Container:pool_add_str(allContainersCollection, name);
    containerItemCollection[index] = pool_new(capacity);

    if (call) {
        CallLocalFunction("OnContainerCreate", "i", _:index);
    }

    return index;
}

stock bool:IsValidContainer(Container:containerid) {
    return _:containerid >= 0 && pool_has(allContainersCollection, _:containerid);
}

stock bool:SetContainerName(Container:containerid, const name[]) {
    if (!IsValidContainer(containerid)) {
        return false;
    }
    pool_set_str(allContainersCollection, containerid, name);
    return true;
}

stock bool:GetContainerName(Container:containerid, output[], size = sizeof (output)) {
    if (!IsValidContainer(containerid)) {
        return false;
    }
    pool_get_str(allContainersCollection, containerid, output, size);
    return true;
}

stock Pool:GetContainerPool() {
    return allContainersCollection;
}

stock bool:AddItemToContainer(Container:containerid, Item:itemid, &index = -1, addToIndex = -1, playerid = INVALID_PLAYER_ID, bool:call = true) {
    if (!IsValidContainer(containerid)) {
        return false;
    }
    if (!IsValidItem(itemid)) {
        return false;
    }
    if (itemContainerID[itemid] != INVALID_CONTAINER_ID) {
        return false;
    }

    if (addToIndex >= 0 && !pool_has(containerItemCollection[containerid], addToIndex)) {
        pool_set(containerItemCollection[containerid], addToIndex, itemid);
        index = addToIndex;
    } else {
        index = pool_add(containerItemCollection[containerid], itemid);
    }

    itemContainerID[itemid] = containerid;
    itemContainerIndex[itemid] = index;

    if (call) {
        CallLocalFunction("OnItemAddToContainer", "iiii", _:containerid, _:itemid, index, playerid);
    }

    return true;
}

stock bool:RemoveItemFromContainer(Container:containerid, index, &Item:itemid = INVALID_ITEM_ID, playerid = INVALID_PLAYER_ID, bool:call = true) {
    if ((itemid = GetContainerSlotItem(containerid, index)) == INVALID_ITEM_ID) {
        return false;
    }

    itemContainerID[itemid] = INVALID_CONTAINER_ID;
    itemContainerIndex[itemid] = -1;

    pool_remove(containerItemCollection[containerid], index);
    
    if (call) {
        CallLocalFunction("OnItemRemoveFromContainer", "iiii", _:containerid, _:itemid, index, playerid);
    }

    return true;
}

stock bool:IsContainerSlotUsed(Container:containerid, index) {
    if (!IsValidContainer(containerid)) {
        return false;
    }
    return index >= 0 && pool_has(containerItemCollection[containerid], index);
}

stock Item:GetContainerSlotItem(Container:containerid, index) {
    if (!IsContainerSlotUsed(containerid, index)) {
        return INVALID_ITEM_ID;
    }
    return Item:pool_get(containerItemCollection[containerid], index);
}

stock Iter:GetContainerIter(Container:containerid, index = 0) {
    if (!IsValidContainer(containerid)) {
        return INVALID_ITERATOR;
    }
    
    new const Iter:iter = pool_iter(containerItemCollection[containerid], index);
    iter_acquire(iter);
    return iter;
}

hook OnGameModeInit() {
    allContainersCollection = pool_new(_:MAX_CONTAINERS);
    
    return 0;
}