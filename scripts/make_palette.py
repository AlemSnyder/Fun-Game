import json


def to_string(hex_color):
    r = str(int(hex_color[0:2], 16))
    g = str(int(hex_color[2:4], 16))
    b = str(int(hex_color[4:6], 16))

    s = f"{r:>3} {g:>3} {b:>3}     #{hex_color.upper()}"

    return s


with open("./data/materials.json", "r") as materials:
    data = json.load(materials)

colors = []
for material in data:
    for color in data[material]["colors"]:
        colors.append(color["hex"][0:-2])

print("GIMP Palette\nName: A_Palette\n#")
for c in colors:
    print(to_string(c))
