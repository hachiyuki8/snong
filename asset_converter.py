from PIL import Image
from numpy import asarray

TILE_SIZE = 8
COLOR_LIMIT = 4

IMAGE_PATH = "images/"
SNAKE_HEAD_PATH = IMAGE_PATH + "snake_head.png"
SNAKE_BODY_PATH = IMAGE_PATH + "snake_body.png"

DATA_PATH = "dist/"
SNAKE_HEAD_DATA = DATA_PATH + "snake_head.dat"
SNAKE_BODY_DATA = DATA_PATH + "snake_body.dat"

ASSETS = {SNAKE_HEAD_PATH: SNAKE_HEAD_DATA, SNAKE_BODY_PATH: SNAKE_BODY_DATA}

def make_file_from_image(image_path: str):
    """
    input image must be TILE_SIZE * TILE_SIZE and uses most COLOR_LIMIT distinct colors
    """
    
    # https://www.pluralsight.com/guides/importing-image-data-into-numpy-arrays
    snake_head = Image.open(image_path)
    assert snake_head.size == (TILE_SIZE, TILE_SIZE)
    data = asarray(snake_head)

    colors = []
    pixel_to_color = [[0] * len(data[0]) for _ in range(len(data))]
    for x in range(len(data)):
        for y in range(len(data[0])):
            color = tuple(data[x][y])
            try:
                idx = colors.index(color)
            except:
                assert(len(colors) < COLOR_LIMIT)
                colors.append(color)
                idx = colors.index(color)
            finally: 
                # map position of the current pixel to the index of its color in the palette
                pixel_to_color[x][y] = idx
    colors.extend([(0,0,0,0)] * (COLOR_LIMIT-len(colors)))

    tile_data = [[], []]
    for row in pixel_to_color[::-1]:
        bit0 = 0
        bit1 = 0
        for col in row:
            bit0 = (bit0 << 1) | col & 1
            bit1 = (bit1 << 1) | ((col >> 1) & 1)
        tile_data[0].append(bit0)
        tile_data[1].append(bit1)

    # testing for data integrity
    for x in range(TILE_SIZE):
        bit0 = tile_data[0][x] 
        bit1 = tile_data[1][x]
        for y in range(TILE_SIZE):
            color = (((bit1 >> y) & 1) << 1) | ((bit0 >> y) & 1)
            assert(color == pixel_to_color[TILE_SIZE-1-x][TILE_SIZE-1-y])
    
    return (tile_data, colors)

# https://stackoverflow.com/questions/19142491/how-to-write-a-list-of-integers-to-a-binary-file-in-python
def write_to_binary(tile, palette, output_file):
    with open(output_file, "wb") as f:
        f.write(bytearray(tile[0]))
        f.write(bytearray(tile[1]))
        for color in palette:
            f.write(bytearray(color))

def main():
    for image_path, output_path in ASSETS.items():
        tile, palette = make_file_from_image(image_path)
        write_to_binary(tile, palette, output_path)

        # testing for data integrity
        with open(output_path, "rb") as f:
            data = f.read()
            tile0 = data[:TILE_SIZE]
            tile1 = data[TILE_SIZE:2*TILE_SIZE]
            colors = data[-COLOR_LIMIT*4:]
            assert([x for x in tile0] == tile[0])
            assert([x for x in tile1] == tile[1])
            for i, color in enumerate(palette):
                assert(list(color) == [x for x in colors[i*4:(i+1)*4]])

if __name__ == "__main__":
    main()