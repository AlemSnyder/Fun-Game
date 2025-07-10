# Tile

Basically raw data. Exists in chunks saved in world.

# Object

Anything with a specific render

# Tile Object

Implementation for something that doesn't move.

The expectation is that this will be updated less often

# Entity

This is the implementation if something does move.

The expectation is this will be updated every frame.

# Entity structure

Unique pointer to entity instance in a chunk

once per frame some chunks get their entities copied into the big entity

No entity ID entity doesn't know where entity instances are, the instances do.

fundamentally very different from tile objects which don't move as much.
