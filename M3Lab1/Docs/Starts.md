# Starts

A start entry has the following format:

```nohighlight
{NAME}
{DESCRIPTION}

{HP};{MANA};{MANA_REGEN};{ARMOR};

{SKILL_NAME}:{SKILL_VALUE},
END:

{ACTION_NAME},
END,

{CLASS_NAME}:{CLASS_LEVEL},
END:

{PERK_NAME},
END,

{ITEM_ID}x{ITEM_COUNT};{ITEM_METADATA},
END,

{TURNS};{START_ROOM_ID};{XP};{GOLD};

{DELIM}
```

Name          | Value   | Description
:-------------|:-------:|:-----------
NAME          | String  | The name of the start.
DESCRIPTION   | String  | The description of the start.
HP            | Integer | The player's starting HP.
MANA          | Integer | The player's starting Mana.
MANA_REGEN    | Integer | The player's starting Mana Regeneration.
ARMOR         | Integer | The player's starting Armor.
SKILL_NAME    | String  | The name of the skill.
SKILL_VALUE   | Integer | The level in the skill.
ACTION_NAME   | String  | The name of the action.
CLASS_NAME    | String  | The name of the class.
CLASS_LEVEL   | Integer | The level in the class.
PERK_NAME     | String  | The name of the perk.
ITEM_ID       | Integer | The id of the item.
ITEM_COUNT    | Integer | The size of the item stack.
ITEM_METADATA | String  | The metadata of the item.
TURNS         | Integer | The player's starting Turns.
START_ROOM_ID | Integer | The id of the room the player starts in.
XP            | Integer | The player's starting XP.
GOLD          | Integer | The player's starting Gold.
DELIM         | String  | CONTINUE if there is another entry in the file, FINAL to finish the parsing.

There can be any number of `{SKILL_NAME}:{SKILL_VALUE},` before the `END:` for the skills.

There can be any number of `{ACTION_NAME},` before the `END,` for the actions.

There can be any number of `{CLASS_NAME}:{CLASS_LEVEL},` before the `END:` for the classes.

There can be any number of `{PERK_NAME},` before the `END,` for the perks.

There can be any number of `{ITEM_ID}x{ITEM_COUNT};{ITEM_METADATA},`before the `END,` for items.

The file should always end with a `FINAL` delim.
