import os
import re
from urllib.parse import \
    quote_plus  # for some reason the concatenation logic didn't work, so chatgpt told me to import this

import requests
from tqdm import tqdm

# Used Google api to list the name of each font + Then saved all of that into a txt called "fonts.txt"
FONT_LIST_PATH = "/Users/rafi/Library/Application Support/JetBrains/PyCharm2024.3/scratches/fonts.txt"
DOWNLOAD_DIR = "downloaded_fonts"
CSS_URL_TEMPLATE = "https://fonts.googleapis.com/css?family={font_name}"

os.makedirs(DOWNLOAD_DIR, exist_ok=True)


def extract_first_font_url(css_content):
    """
    Parses the CSS content to find the first font URL (any format). .tff is preferred

    Args:
        css_content (str): The CSS content fetched from Google Fonts.

    Returns:
        tuple or None: A tuple containing the font URL and its format, or None if no URL is found.
    """
    # horrifying re
    font_url_match = re.search(
        r'src:\s*url\((https?://[^)]+)\)\s*format\([\'"]?([^\'")]+)[\'"]?\)', css_content
    )
    if font_url_match:
        return font_url_match.group(1), font_url_match.group(2)
    return None


def sanitise_filename(filename):
    """
    Sanitizes the filename by removing characters that are invalid in file names.

    Args:
        filename (str): The original filename.

    Returns:
        str: The sanitized filename.
    """
    # Remove characters not allowed in filenames, caused some issues for me at least on windows
    return re.sub(r'[<>:"/\\|?*]', '', filename)


def download_font(font_name, font_url, font_format, download_dir):
    """
    Downloads the font from the given URL and saves it with the font's name.

    Args:
        font_name (str): The name of the font.
        font_url (str): The URL to download the font from.
        font_format (str): The format of the font (e.g., 'woff2', 'ttf').
        download_dir (str): The directory where the font will be saved.
    """
    try:
        response = requests.get(font_url, timeout=10)

        extension_mapping = {
            'woff2': '.woff2',
            'truetype': '.ttf',
        }
        extension = extension_mapping.get(font_format.lower(), '.ttf')  # defaults to tff, but woff (2.0) is common too + I prefer tff
        safe_font_name = sanitise_filename(font_name)
        file_name = f"{safe_font_name}{extension}" # You get a weird random string otherwise
        file_path = os.path.join(download_dir, file_name)

        with open(file_path, "wb") as f:
            f.write(response.content)

        print(f"Downloaded: {file_name}")

    except requests.exceptions.RequestException:
        print(f"Failed to download '{font_name}' from '{font_url}'")

def main():
    with open(FONT_LIST_PATH, "r", encoding="utf-8") as f:
        fonts = [line.strip() for line in f if line.strip()]

    with tqdm(total=len(fonts), desc="Downloading Fonts", unit="font") as pbar:
        for font_name in fonts:
            # URL-encode the font name using quote_plus to replace spaces with '+', again Chatgpt, still confused why it wasn't working
            encoded_font_name = quote_plus(font_name)
            css_url = CSS_URL_TEMPLATE.format(font_name=encoded_font_name)

            try:
                headers = {
                    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64)" # just a standard, think it's cap
                }
                css_response = requests.get(css_url, headers=headers, timeout=10)
                css_response.raise_for_status()
                css_content = css_response.text

                result = extract_first_font_url(css_content)

                if result:
                    font_url, font_format = result
                    download_font(font_name, font_url, font_format, DOWNLOAD_DIR)
                else:
                    print(f"No valid font URL found for '{font_name}'.")

            except requests.exceptions.RequestException:
                print(f"Failed to fetch CSS for '{font_name}'.")

            pbar.update(1)


if __name__ == "__main__":
    main()
