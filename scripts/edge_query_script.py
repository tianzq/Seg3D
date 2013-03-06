# run script using exec(open('../scripts/edge_query_script.py').read()) (change to Windows path if necessary)

import os, sys

# must add Seg3D location to python path
seg3d_location='/Users/aylakhan/devel/seg3d2_is/bin'
#seg3d_location='/Applications/Seg3D2.app/Contents/MacOS'
#matlab_script_location='/Users/aylakhan/devel/seg3d2_is/bin'
matlab_script_location='/Users/aylakhan/devel/seg3d2_is'
data_dir='/Users/aylakhan/Downloads/SampleData'

# TODO: move to function
if not os.path.exists(seg3d_location):
  print("Path %s to Seg3D does not exist." % seg3d_location)
  sys.exit(0)

if not os.path.exists(matlab_script_location):
  print("Path %s to Matlab python scripts does not exist." % matlab_script_location)
  sys.exit(0)

if not os.path.exists(data_dir):
  print("Path %s to data does not exist." % data_dir)
  sys.exit(0)

# TODO: move to function
if seg3d_location not in sys.path:
  sys.path.append(seg3d_location)

if matlab_script_location not in sys.path:
  sys.path.append(matlab_script_location)

import mlabraw3
import seg3d2

# check for file first
dataLayerID = importseries(filenames=os.path.join(data_dir, 'original_gray.jpg'), importer='[ITK FileSeries Importer]', mode='data')
print(dataLayerID)

#session = mlabraw3.open()
#print('Matlab session opened')

verticesFromFile = []

# test
counter = 0
while counter < 3:
# stops looping when points file is empty
#while True:

  # matlab eval call
  # TODO: replace with actual Matlab function
  # TODO: error handling
  #print("Call Matlab")
  #mlabraw3.eval(session, 'A=zeros(4, 4)', log=True)


  # assuming layer mask will always have the same name...
  # TODO: should check file for changes - checksum?
  maskLayerID = importlayer(filename=os.path.join(data_dir, 'label_mask.mat'), importer='[Matlab Importer]', mode='single_mask')
  print(maskLayerID)

  # set up Axial view by default
  # returns bool
  result = set(stateid='view::active_viewer', value=0)

  # returns bool
  result=set(stateid='viewer0::view_mode', value='Axial')

  # returns bool
  result=set(stateid='viewer0::flip_vertical', value='true')

  # check for points file
  # poll...
  pointsFile = open(os.path.join(data_dir, 'points.txt'))
  for line in pointsFile:
    floats = [float(points) for points in line.split()]
    verticesFromFile.append(floats)

  pointsFile.close()

  if not verticesFromFile:
    print("Done")
    break

  # compare against last iteration and break if same???

  # 2D case - add default z location
  for pointList in verticesFromFile:
    if len(pointList) == 2:
      pointList.append(0)

  print("Edge query vertices from Matlab: ", verticesFromFile)

  toolid=opentool(tooltype='edgequerytool')
  vertices_stateid=toolid+"::vertices"
  #set(stateid=vertices_stateid, value=[[55,34,0],[67,165,0],[23,90,0]])
  set(stateid=vertices_stateid, value=verticesFromFile)

  # axial = 0, coronal = 1, sagittal = 2
  # test points same as points matrix
  #edgequery(target=maskLayerID, slice_type=0, slice_number=86, vertices=[[55,34,0],[67,165,0],[23,90,0]])
  #edgequery(target=maskLayerID, slice_type=0, slice_number=86, vertices=verticesFromFile)

  # wait for save button...
  # terrible, but necessary for now
  while (True):
    save_stateid=toolid+"::save"
    get(stateid=save_stateid)
    if True == get(save_stateid):
      break

  # test
  # selected edge label will come from edgequery tool

  # need to change to space delimited; 0 1 or 1 0
  #edgeLabel = 'L2'

  # query edge...
  #selectedEdge = get(stateid=toolid)
  edgeFile = open(os.path.join(data_dir, 'edges.txt'), 'wt')
  edgeFile.write(edgeLabel)
  edgeFile.close()

  # TODO: should layer be deleted?
  #deletelayers(layers=maskLayerID)
  # delete edge query

  counter=counter+1

#mlabraw3.close(session)
#print('Matlab session closed')