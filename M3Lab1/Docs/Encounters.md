# Encounters

An encounter entry has the following format:

```nohighlight
{WEIGHT}

{EFFECT}

{DELIM};
```

Name   | Value      | Description
:------|:----------:|:-----------
WEIGHT | Sockscript | A sockscript block that must return an int32 to serve as the relative weight of the encounter.
EFFECT | Sockscript | A sockscript block that runs the effect of the encounter, does not need to return a value.
DELIM  | String     | CONTINUE if there is another entry in the file, FINAL to finish the parsing.

The file should always end with a `FINAL` delim.
