import json

def to_string(hex_color):
    r=str(int(hex_color[0:2],16))
    g=str(int(hex_color[2:4],16))
    b=str(int(hex_color[4:6],16))

    s=" "*(3-len(r))+r+" "*(4-len(g))+g+" "*(4-len(b))+b+"     #" + hex_color.upper()
    return s

with open("./data/materials.json", "r") as materials:
    data = json.load(materials)

colors = []
for material in data:
    for color in data[material]["colors"]:
        #print(color)
        colors.append(color["hex"][0:-2])

print("GIMP Palette\nName: A_Palette\n#")
for c in colors:
    print(to_string(c))
