import struct
import argparse

def gen_smbios_bin(yaml_file, bin_file):
    with open(yaml_file, 'r') as f:
        # Read and filter YAML file: remove empty lines, comments (starting with #),
        # and the YAML header line (smbios_strings:)
        lines = [line.strip() for line in f.readlines()
                if line.strip() and not line.strip().startswith('#')
                and not line.strip().startswith('smbios_strings:')]

    with open(bin_file, 'wb') as out:
        i = 0
        while i < len(lines):
            # Assume each entry has exactly 3 lines: - type:, idx:, string
            type_val = int(lines[i].split(':')[1].split('#')[0].strip())
            idx_val = int(lines[i+1].split(':')[1].split('#')[0].strip())
            string_val = lines[i+2].split('"')[1]  # Assumes string is quoted

            out.write(struct.pack('BB', type_val, idx_val))
            out.write(string_val.encode('ascii') + b'\0')

            # If this is the end-of-table marker (Type 127), we're done
            if type_val == 127:
                break

            i += 3  # Skip to next entry


def main():
    parser = argparse.ArgumentParser(description='Generate smbios.bin from SMBIOS YAML file')
    parser.add_argument('-i', '--input_file', required=True, help='Path to SMBIOS YAML file')
    parser.add_argument('-o', '--output_file', default='smbios.bin', help='Output file path for smbios.bin (default: smbios.bin in current directory)')
    args = parser.parse_args()

    yaml_file = args.input_file
    bin_file = args.output_file
    gen_smbios_bin(yaml_file, bin_file)
    print(f'Generated {bin_file} from {yaml_file}')

if __name__ == '__main__':
    main()
