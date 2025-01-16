import re
import matplotlib.pyplot as matplt
from collections import defaultdict

#Initializing address range in integer
base_address = int('0xae40000000', 16)
end_address = int('0xae80000000', 16)
size_of_region = 2 * 1024 * 1024           #2MB in bytes

#Dictionary to count TLB misses(L2 miss count) per region
regions = defaultdict(int)

#Function for identifying if an address falls within the range
def get_region_number(address):
    if base_address <= address <= end_address:
        return (address - base_address) // size_of_region
    return None

#Counting misses if address falls into a valid region
with open('address_2.txt', 'r') as file:
    for line in file:
        match = re.search(r'\[\.]\s*(0x[0-9a-fA-F]+)\s+anon\s+None\s+L2 miss', line)
        
        if match:
            hex_address = match.group(1)
            address = int(hex_address, 16)
            region_no = get_region_number(address)
            if region_no is not None:
                regions[region_no] += 1

#Sorting the dictionary by number of TLB misses(L2 misses)
sorted_regions = sorted(regions.items(), key=lambda x: x[1], reverse=True)

#Storing top 'n' TLB misses(L2 misses) regions and storing it in an array 
n = 8 
top_n_regions = []
print(f"\nTop {n} regions with the most L2 misses:")
for regions, count in sorted_regions[:n]:
    start_address = base_address + regions * size_of_region
    decimal_address = start_address
    top_n_regions.append(decimal_address) 
    print(f"Region {regions}: Starting Address: {hex(start_address)}: {count} L2 misses ")

with open('largepages.txt', 'w') as output_file:
    for address in top_n_regions:
        output_file.write(f"{address}\n")

# Plotting graph for TLB misses(L2 misses)
if sorted_regions:
    regions, counts = zip(*sorted_regions)
    max_region = max(regions)
    xticks_range = list(range(0, max_region + 1, 100))

    matplt.bar(regions, counts)
    matplt.xlabel('Region (2MB blocks)')
    matplt.ylabel('L2 Miss Count')
    matplt.title('L2 Miss Count per 2MB Region')
    matplt.xticks(xticks_range)
    matplt.savefig('graph_before.png')
    matplt.show()

