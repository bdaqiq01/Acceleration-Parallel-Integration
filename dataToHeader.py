import sys
import pandas as pd

def extract_column_to_header(input_csv, output_header, col_index):
    try:
        df = pd.read_csv(input_csv)
    except Exception as e:
        print(f"Error reading CSV: {e}")
        sys.exit(1)

    if col_index >= len(df.columns):
        print(f"Invalid column index. File only has {len(df.columns)} columns.")
        sys.exit(1)

    values = df.iloc[:, col_index].dropna().astype(float).tolist()

    with open(output_header, 'w') as out:
        out.write("// Auto-generated from CSV file\n")
        out.write("double DefaultProfile[] = {\n")
        for i, val in enumerate(values):
            out.write(f"    {val:.15f},")
            if (i + 1) % 9 == 0:
                out.write("\n")
        out.write("\n};\n")

    print(f"Wrote {len(values)} values to {output_header}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python csv_to_header.py <input.csv> <output.h> <column_index>")
        sys.exit(1)

    input_csv = sys.argv[1]
    output_header = sys.argv[2]
    col_index = int(sys.argv[3])

    extract_column_to_header(input_csv, output_header, col_index)
