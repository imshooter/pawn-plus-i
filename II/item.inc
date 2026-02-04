#if defined _item_included
    #endinput
#endif
#define _item_included

#include <pp-hooks>
#include <uuid>

#define MAX_ITEMS (Item:100000)
#define MAX_ITEM_BUILDS (ItemBuild:100000)

#define MAX_ITEM_EXTRA_NAME (32)
#define MAX_ITEM_BUILD_NAME (32)
#define MAX_ITEM_NAME (MAX_ITEM_BUILD_NAME + MAX_ITEM_EXTRA_NAME)

#define MAX_ITEM_KEY_LENGTH (32)
#define MAX_ITEM_EXTRA_DATA (64)

#define INVALID_ITEM_ID (Item:-1)
#define INVALID_ITEM_BUILD (ItemBuild:-1)

static enum _:E_ITEM_DATA {
    Float:E_ITEM_X,
    Float:E_ITEM_Y,
    Float:E_ITEM_Z,
    Float:E_ITEM_ROTATION_X,
    Float:E_ITEM_ROTATION_Y,
    Float:E_ITEM_ROTATION_Z,
    E_ITEM_WORLD_ID,
    E_ITEM_INTERIOR_ID,

    Button:E_ITEM_BUTTON_ID,
    STREAMER_TAG_OBJECT:E_ITEM_OBJECT_ID,

    String:E_ITEM_UUID,
    String:E_ITEM_EXTRA_NAME,

    // Items Extra Data Collection
    Map:E_ITEM_EXTRA_DATA_MAP
};

static item[MAX_ITEMS][E_ITEM_DATA];

// Items Collection
static Pool:allItemsCollection;

/**
 * @Functions
 */

forward Item:CreateItem(ItemBuild:buildid, const UUID[] = "", bool:call = true);
forward bool:DestroyItem(Item:itemid, bool:call = true);
forward bool:IsValidItem(Item:itemid);
forward ItemBuild:GetItemBuild(Item:itemid);
forward bool:SetItemExtraName(Item:itemid, const name[]);
forward bool:GetItemExtraName(Item:itemid, output[], size = sizeof (output));
forward bool:GetItemUUID(Item:itemid, output[], size = sizeof (output));
forward bool:HasItemAnyExtraData(Item:itemid);
forward bool:HasItemExtraData(Item:itemid, const key[]);
forward bool:SetItemExtraData(Item:itemid, const key[], value);
forward bool:GetItemExtraData(Item:itemid, const key[], &value);
forward Pool:GetItemPool();

/**
 * @External
 */

stock Item:CreateItem(ItemBuild:buildid, const UUID[] = "", bool:call = true) {
    if (pool_size(allItemsCollection) >= _:MAX_ITEMS) {
        return INVALID_ITEM_ID;
    }

    new const Item:index = Item:pool_add(allItemsCollection, buildid);

    item[index][E_ITEM_UUID] = (UUID[0] == EOS)
        ? str_acquire(UUID_s())
        : str_acquire(str_new(UUID));

    if (call) {
        CallLocalFunction("OnItemCreate", "i", _:index);
    }

    return index;
}

stock bool:DestroyItem(Item:itemid, bool:call = true) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    if (str_valid(item[itemid][E_ITEM_UUID])) str_delete(item[itemid][E_ITEM_UUID]);
    if (str_valid(item[itemid][E_ITEM_EXTRA_NAME])) str_delete(item[itemid][E_ITEM_EXTRA_NAME]);
    if (map_valid(item[itemid][E_ITEM_EXTRA_DATA_MAP])) map_delete(item[itemid][E_ITEM_EXTRA_DATA_MAP]);

    item[itemid][E_ITEM_UUID] = STRING_NULL;
    item[itemid][E_ITEM_EXTRA_NAME] = STRING_NULL;
    item[itemid][E_ITEM_EXTRA_DATA_MAP] = INVALID_MAP;

    pool_remove(allItemsCollection, _:itemid);
    if (call) {
        CallLocalFunction("OnItemDestroy", "i", _:itemid);
    }

    return true;
}

stock bool:IsValidItem(Item:itemid) {
    return _:itemid >= 0 && pool_has(allItemsCollection, _:itemid);
}

stock ItemBuild:GetItemBuild(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return INVALID_ITEM_BUILD;
    }
    return ItemBuild:pool_get(allItemsCollection, _:itemid);
}

stock bool:SetItemExtraName(Item:itemid, const name[]) {
    if (!IsValidItem(itemid)) {
        return false;
    }
    if (str_valid(item[itemid][E_ITEM_EXTRA_NAME])) {
        str_delete(item[itemid][E_ITEM_EXTRA_NAME]);
    }
    item[itemid][E_ITEM_EXTRA_NAME] = str_acquire(str_new(name));
    return true;
}

stock bool:GetItemExtraName(Item:itemid, output[], size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        return false;
    }
    str_get(item[itemid][E_ITEM_EXTRA_NAME], output, size);
    return true;
}

stock bool:SetItemUUID(Item:itemid, const UUID[]) {
    if (!IsValidItem(itemid)) {
        return false;
    }
    if (str_valid(item[itemid][E_ITEM_UUID])) {
        str_delete(item[itemid][E_ITEM_UUID]);
    }
    item[itemid][E_ITEM_UUID] = str_acquire(str_new(UUID));
    return true;
}

stock bool:GetItemUUID(Item:itemid, output[], size = sizeof (output)) {
    if (!IsValidItem(itemid)) {
        return false;
    }
    str_get(item[itemid][E_ITEM_UUID], output, size);
    return true;
}

// Extra Data
stock bool:HasItemAnyExtraData(Item:itemid) {
    if (!IsValidItem(itemid)) {
        return false;
    }

    return item[itemid][E_ITEM_EXTRA_DATA_MAP] != INVALID_MAP && map_size(item[itemid][E_ITEM_EXTRA_DATA_MAP]) != 0;
}

stock bool:HasItemExtraData(Item:itemid, const key[]) {
    if (!HasItemAnyExtraData(itemid)) {
        return false;
    }

    return map_has_str_key(item[itemid][E_ITEM_EXTRA_DATA_MAP], key);
}

stock bool:SetItemExtraData(Item:itemid, const key[], value) {
    if (!IsValidItem(itemid)) {
        return false;
    }
    if (!(0 < strlen(key) < MAX_ITEM_KEY_LENGTH)) {
        return false;
    }
    if (item[itemid][E_ITEM_EXTRA_DATA_MAP] == INVALID_MAP) {
        item[itemid][E_ITEM_EXTRA_DATA_MAP] = map_new();
    }
    if (!map_has_str_key(item[itemid][E_ITEM_EXTRA_DATA_MAP], key) && map_size(item[itemid][E_ITEM_EXTRA_DATA_MAP]) >= MAX_ITEM_EXTRA_DATA) {
        return false;
    }
    map_str_set(item[itemid][E_ITEM_EXTRA_DATA_MAP], key, value);
    return true;
}

stock bool:GetItemExtraData(Item:itemid, const key[], &value) {
    if (!HasItemAnyExtraData(itemid)) {
        return false;
    }

    return map_str_get_safe(item[itemid][E_ITEM_EXTRA_DATA_MAP], key, value);
}

stock Pool:GetItemPool() {
    return allItemsCollection;
}

hook OnGameModeInit() {
    allItemsCollection = pool_new(_:MAX_ITEMS);
    
    return 0;
}