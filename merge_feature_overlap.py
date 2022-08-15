#%%
import sys,json

#%%
# read the first line of traits.txt
with open(sys.argv[1]) as f:
  first_line = f.readline()
  feature_overlap = float(first_line)

# merge json files
json_file = sys.argv[2]
with open(json_file) as f:
  data = json.load(f)
  data['FeatureOverlap'] = feature_overlap

with open(json_file, 'w') as f:
  json.dump(data, f, indent=2)

