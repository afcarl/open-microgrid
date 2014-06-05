from PIL import Image, ImageDraw

colors = [(0, 0, 0),
          (40, 40, 40),
          (90, 90, 90),
          (150, 150, 150),
          (255, 255, 255)]
"""
codes = ['284a52cf56753b8baab74acc5f07a643',
         '0bb5daf5c014962fb3df37b46753fb82',
         'c902a2662ca87f91f04fc0ee1c1f385d',
         '3aa40dc0eaaed7b332fb57fd3e918bb8',
         '2bf3c2713f3a300adafb9e3fcb3200ca',
         '3cfc5de67bf3c12912200cdabcc4a543',
         '0da76c2d67d65a608567b70984e1ac84',
         'fb209f4f9c16df4e847cde3c49d0de69',
         '18e89677066bbe0db935cbdc927c1ae9',
         '49997c426a7849eeb888782e2f9e2fc1',
         '36b28404d02af53de390277f8c73013a',
         '1f0ff49c5f1d14b29876b18d626fffae',
         '865834f3ebe9af9c21a1265d3921f39c',
         'c0b16b276c03eeb7a4b3a070b669674a',
         'f386665080e62d1b2aea65949f35c77c',
         'e8af11986cc7fd06b9b77ec2f05d9ad8',
         '1b9e68d331fd4301cedd1439dc5d4391',
         '07bb4be27693972d62561188a2891cd5',
         '58e2c2b37771608796fa113d476edbad',
         '56d59b27e03780823e9256a7938b0204']
"""

codes = ['284a52cf',
         '0bb5daf5',
         'c902a266',
         '3aa40dc0',
         '2bf3c271',
         '3cfc5de6',
         '0da76c2d',
         'fb209f4f',
         '18e89677',
         '49997c42',
         '36b28404',
         '1f0ff49c',
         '865834f3',
         'c0b16b27',
         'f3866650',
         'e8af1198',
         '1b9e68d3',
         '07bb4be2',
         '58e2c2b3',
         '56d59b27']


def draw_code(dr, pos, code):
    w, h = pos
    def put_rectange(rect_color):
        nonlocal w
        dr.rectangle([ (w, h), (w+30, h+35)], rect_color)
        w+=30

    for init_color in [1,5,2,4,3,5]:
        put_rectange(colors[init_color-1])

    number = int(code, base=16)
    base_four = []
    while number > 0:
        base_four.append(number%4)
        number //= 4
    while len(base_four) < 2*len(code):
        base_four.append(0)
    base_four.reverse()

    parity = sum(base_four) % 4
    base_four.append(parity)

    last_color = 4
    num_stripes = 0
    code_colors = []
    for digit in base_four:
        next_color = (last_color + digit + 1) % 5
        code_colors.append(next_color)
        put_rectange(colors[next_color])
        num_stripes += 1
        last_color = next_color
    print("Code colors = %s" % code_colors)
    print("printed %d stripes" % num_stripes)

im = Image.new("RGB", (900, 1224), "white")
dr = ImageDraw.Draw(im);
for i, code in enumerate(codes):
    dr.text((30, 40*i+35), code, (0,0,0))
    w, _ = dr.textsize(code)
    draw_code(dr, (w+50, 40*i+30), code)

im.save("codes.jpg");