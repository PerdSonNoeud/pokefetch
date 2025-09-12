import os
import sys
import time
from io import BytesIO

import requests
from PIL import Image

start = time.time()

def loading_bar(iteration, total, length=30, flavour=""):
    """
    function that create loading bar in the console.
    Might need it later
    """
    percent = iteration / total
    bar = "━" * int(length * percent) + "-" * (length - int(length * percent))
    timer = time.time() - start
    sys.stdout.write(f"\r{timer:.2f}s - [{bar}] {percent * 100:.1f}% {flavour if flavour != '' else ' '*10}")
    sys.stdout.flush()
    time.sleep(0.01)


def getImage(url):
    try:
        response = requests.get(url)
        response.raise_for_status()
        img = Image.open(BytesIO(response.content)).convert("RGBA")
    
        return img
    except:
        print(f"error trying to request from : {url}")
        return None


def get_colors(img, gray: False):
    size = img.getbbox()
    colors = []
    for y in range(size[1], size[3] + 1):
        colors.append([])
        for x in range(size[0], size[2] + 6):
            if x < img.size[0] and y < img.size[1]:
                pixel = img.getpixel((x, y))
                if gray:
                    r = g = b = 100
                else:
                    r = pixel[0]
                    g = pixel[1]
                    b = pixel[2]
                a = pixel[3]
                if a == 0:
                    colors[y - size[1]].append("\x1b[0m")
                else:
                    colors[y - size[1]].append(f"2;{r};{g};{b}m")
            else:
                colors[y - size[1]].append("\x1b[0m")
    return colors


def convert_to_text(colors):
    text = ""
    for y in range(len(colors) // 2):
        for x in range(len(colors[y])):
            if colors[y * 2][x] == "\x1b[0m":
                if colors[y * 2 + 1][x] == "\x1b[0m":
                    text += " "
                else:
                    text += f"\x1b[38;{colors[y * 2 + 1][x]}▄\x1b[0m"
            else:
                if colors[y * 2 + 1][x] == "\x1b[0m":
                    text += f"\x1b[38;{colors[y * 2][x]}▀\x1b[0m"
                else:
                    text += f"\x1b[38;{colors[y * 2][x]}\x1b[48;{colors[y * 2 + 1][x]}▀\x1b[0m"
        text += "\n"
    return text


def savefile(name, text, shiny):
    if name == "unknown":
        with open(f"assets/icons/{name}.txt", "w") as f:
            f.write(text)
        return

    with open(f"assets/icons/{'shiny' if shiny else 'regular'}/{name}.txt", "w") as f:
        f.write(text)


if __name__ == "__main__":
    json_data = requests.get(
        "https://raw.githubusercontent.com/msikma/pokesprite/master/data/pokemon.json"
    ).json()
    loading_bar(0, json_data.__len__() + 1)
    n = total = 0
    for v in list(json_data.values())[:]:
        n += 1
        name = (
            v["name"]["eng"]
            .lower()
            .replace(" ", "-")
            .replace(".", "")
            .replace("'", "")
            .replace(":", "")
            .replace("♀", "-f")
            .replace("♂", "-m")
            .replace("é", "e")
        )
        shiny = False
        for i in range(2):
            if os.path.exists(f"assets/icons/{'shiny' if shiny else 'regular'}/{name}.txt"):
                total += 1
                continue
            img = getImage(
                f"https://github.com/msikma/pokesprite/raw/master/pokemon-gen8/{'shiny' if shiny else 'regular'}/{name}.png"
            )
            if img != None:
                total += 1
                colors = get_colors(img)
                text = convert_to_text(colors)
                savefile(name, text, shiny)
            shiny = True

        num = f"{'0' if n < 100 else ''}{'0' if n < 10 else ''}{n}"
        loading_bar(n, json_data.__len__(), flavour=f"- {num}. {name}" + " "*10)

    if not os.path.exists(f"assets/icons/unknown.txt"):
        img = getImage(
            f"https://github.com/msikma/pokesprite/raw/master/pokemon-gen8/unknown.png"
        )
        if img != None:
            total += 1
            colors = get_colors(img, True)
            text = convert_to_text(colors)
            savefile("unknown", text, False)
    else:
        total += 1
    loading_bar(n, json_data.__len__(), flavour=f"- 000. unknown" + " "*10)

    print(f"\n[info] [{total}/{n*2 + 1}] pokémon icons loaded.")
