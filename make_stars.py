import numpy as np


def random_star():
    star_data = {}

    # random distribution of theta angle
    star_data["theta"] = np.random.uniform(0, 360)

    # arc cos distribution of phi angle (so that stars are distributed evenly)
    random_float = np.random.uniform(-1, 1)
    star_data["phi"] = np.rad2deg(np.arccos(random_float))

    # gamma distribution of brightness
    star_data["brightness"] = np.random.gamma(2, 3)

    # beta distribution of age (coldness)
    star_data["age"] = np.random.beta(3, 5)

    return star_data


for x in range(100):
    print(random_star(), ",")
