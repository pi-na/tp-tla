import json
import argparse


def generate_navbar(navbar):
    color = navbar.get("color", "#333")
    items = navbar.get("items", [])
    items_html = "".join([f"<li>{item}</li>" for item in items])
    return f'<nav style="background-color: {color};"><ul>{items_html}</ul></nav>'


def generate_header(header):
    color = header.get("color", "#444")
    title = header.get("title", "")
    subtitle = header.get("subtitle", "")
    return f'<header style="background-color: {color};"><h1>{title}</h1><h2>{subtitle}</h2></header>'


def generate_hero(hero):
    color = hero.get("color", "#555")
    background_image = hero.get("background_image", "")
    headline = hero.get("headline", "")
    style_attr = f"background-color: {color};"
    if background_image:
        style_attr += f" background-image: url('{background_image}'); background-size: cover; background-position: center;"
    return f'<section class="hero" style="{style_attr}"><h1>{headline}</h1></section>'


def generate_section_photo_text(section):
    color = section.get("color", "#666")
    image = section.get("image", "")
    text = section.get("text", "")
    return f'<section style="background-color: {color}; display: flex; align-items: center; padding: 20px;">' \
           f'<img src="{image}" alt="Imagen" style="max-width: 50%; margin-right: 20px;">' \
           f'<p>{text}</p></section>'


def generate_section_multiple_photos(section):
    color = section.get("color", "#777")
    images = section.get("images", [])
    images_html = "".join([f'<img src="{img}" alt="Imagen" style="max-width: 30%; margin: 5px;">' for img in images])
    return f'<section style="background-color: {color}; text-align: center; padding: 20px;">{images_html}</section>'


def generate_text_section(section):
    color = section.get("color", "#888")
    text = section.get("text", "")
    return f'<section style="background-color: {color}; padding: 20px;"><p>{text}</p></section>'


def generate_contact_section(section):
    color = section.get("color", "#999")
    contact_text = section.get("contact_text", "")
    google_maps = section.get("google_maps", "")
    return f'<section style="background-color: {color}; padding: 20px;">' \
           f'<p>{contact_text}</p>{google_maps}</section>'


def generate_boxes_section(section):
    color = section.get("color", "#aaa")
    boxes = section.get("boxes", [])
    boxes_html = ""
    for box in boxes:
        title = box.get("title", "")
        text = box.get("text", "")
        boxes_html += f'<div class="box" style="border: 1px solid #ccc; margin: 5px; padding: 10px;">' \
                      f'<h3>{title}</h3><p>{text}</p></div>'
    return f'<section style="background-color: {color}; padding: 20px;">' \
           f'<div class="boxes" style="display: flex; flex-wrap: wrap;">{boxes_html}</div></section>'


def generate_html_page(data):
    html_parts = []
    html_parts.append("""
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Página Estática Generada</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 0; }
        nav ul { list-style: none; padding: 10px; display: flex; }
        nav li { margin-right: 15px; }
        header, section { padding: 20px; }
        .hero { color: #fff; text-align: center; }
        .boxes { display: flex; flex-wrap: wrap; }
        .box { flex: 1; min-width: 200px; padding: 10px; margin: 5px; border: 1px solid #ccc; }
    </style>
</head>
<body>
    """)

    if "navbar" in data:
        html_parts.append(generate_navbar(data["navbar"]))
    if "header" in data:
        html_parts.append(generate_header(data["header"]))
    if "hero" in data:
        html_parts.append(generate_hero(data["hero"]))
    if "section_foto_texto" in data:
        html_parts.append(generate_section_photo_text(data["section_foto_texto"]))
    if "section_varias_fotos" in data:
        html_parts.append(generate_section_multiple_photos(data["section_varias_fotos"]))
    if "section_texto" in data:
        html_parts.append(generate_text_section(data["section_texto"]))
    if "section_contacto" in data:
        html_parts.append(generate_contact_section(data["section_contacto"]))
    if "section_recuadros" in data:
        html_parts.append(generate_boxes_section(data["section_recuadros"]))

    html_parts.append("""
</body>
</html>
    """)
    return "\n".join(html_parts)


def main():
    parser = argparse.ArgumentParser(description="Generador de HTML a partir de JSON")
    parser.add_argument("json_file", help="Archivo JSON de entrada")
    parser.add_argument("-o", "--output", default="output.html", help="Archivo HTML de salida")
    args = parser.parse_args()

    try:
        with open(args.json_file, "r", encoding="utf-8") as f:
            data = json.load(f)
    except Exception as e:
        print("Error al leer el archivo JSON:", e)
        return

    html_content = generate_html_page(data)

    try:
        with open(args.output, "w", encoding="utf-8") as f:
            f.write(html_content)
        print(f"HTML generado correctamente en {args.output}")
    except Exception as e:
        print("Error al escribir el archivo HTML:", e)


if __name__ == "__main__":
    main()
