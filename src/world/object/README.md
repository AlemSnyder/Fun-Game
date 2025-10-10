# Tile

Basically raw data. Exists in chunks saved in world.

# Objects

## Object

Anything with a specific render. The render program is crated by reading glsl files with a
`gui::shader::ShaderHandler` type. The `Object` needs a shader program of type
`gui::shader::ShaderProgram_ElementsInstanced` because the `Object` holds most of the data while
the instancing is created by individual `ObjectInstance`s.

Technically, `Object` doesn't hold the data `TileObject` and `Entity` do.

## Tile Object

Implementation for something that doesn't move.

The expectation is that this will be updated less often

## Entity

This is the implementation if something does move.

The expectation is this will be updated every frame.

If I were to give this data:

```
health
position
facing direction
hunger

max heath (not really)
speed?
strength?

```

## LivingEntity (idk this name)

Then make higher level entities with more persistent ais

Want to add some sort of states effect.

```
hunger
max health
speed
strength

```

## PlanningEntity

This one has a plan. Some sort of long term planning.

Between these three the only difference can be what data is saved in the class.

# Entity structure

Unique pointer to entity instance in a chunk

once per frame some chunks get their entities copied into the big entity

No entity ID entity doesn't know where entity instances are, the instances do.

fundamentally very different from tile objects which don't move as much.

# TODO:

Need to call update function

Setup backend (sending information to gpu)
