import csv

def main():
    with open('./product.csv', 'r') as csv:
        line = csv.readline()
        headers = line.strip().split(',')
        values = []

        for line in csv:
            value = ''.join(c for c in line if c not in '",')
            print(value)

main()
