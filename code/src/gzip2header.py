import gzip
import os
from pathlib import Path

# import sys

# prog_call = sys.argv[0]
# prog_location = os.path.split(prog_call)[0]

prog_location = Path(__file__).absolute().parent


class header_gen:
    def __init__(self, name: str, dir: str, data: bytes) -> None:
        """Generate a header file from raw gzip data.

        Args:
            name (str): Name of file to use without file extention
            dir (str): The fullpath to the output directory
            data (bytes): Raw gzip data to put in header
        """
        self.name = name
        self.output_dir = dir
        self.data = data
        self.file_size = len(data)

    @classmethod
    def from_gzip_file(cls, name, output_dir=prog_location, input_dir="C:\\Users\\Marcus Börne\\Downloads"):
        """ClassMethod: Takes already compressed gzip file

        - 'name' is name of file without extentions"""
        filename = os.path.join(input_dir, f"{name}.html.gz")
        with open(filename, "rb") as file:
            return cls(name, output_dir, file.read())

    @classmethod
    def from_html_file(cls, name, output_dir=prog_location, input_dir=prog_location, compression: int = 9):
        """ClassMethod: Takes html file

        - 'name' is name of file without fileextention
        - 'compression' is compressionlevel of gzip"""
        if (compression < 0) or (compression > 9):
            raise ValueError(compression)

        filename = os.path.join(input_dir, f"{name}.html")
        with open(filename, "rb") as file:
            data = gzip.compress(file.read(), compression)
            return cls(name, output_dir, data)

    def format_data(self, data_width: int = 16):
        """Returns list of formated strings

        - 'data_width' is maximum amount of bytes represented on each line"""
        # hex_list = []
        i = 0
        string = ""
        string_list = []

        for item in self.data:
            i += 1
            hex_code = hex(item)
            if len(hex_code) == 3:
                hex_code = hex_code.replace("0x", "0x0")
            string += f" {hex_code},"

            if i == data_width:
                string_list.append(string)
                i = 0
                string = ""
        string_list.append(string)
        return string_list

    def make_file(self):
        """Shape and make the header file. Will overwrite previous file!"""
        list = self.format_data(data_width=16)
        list[-1] = list[-1].rstrip(",") + " };"
        filename = os.path.join(self.output_dir, f"{self.name}.h")
        with open(filename, "w") as file:
            curly_bracket = "{"
            file.write(f"\n//File: {self.name}.html.gz, Size: {self.file_size}\n")
            file.write(f"#define {self.name}_html_gz_len {self.file_size}\n")
            file.write(f"const uint8_t {self.name}_html_gz[] = {curly_bracket}\n")
            [file.write(f"{line}\n") for line in list]


def main(src_dir=prog_location):
    gen = header_gen.from_html_file("web_site", src_dir, src_dir, compression=9)
    gen.make_file()


if __name__ == "__main__":
    main()
