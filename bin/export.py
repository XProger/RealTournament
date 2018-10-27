import bpy
import struct
import bmesh
import mathutils

from mathutils import Matrix

MatGL = Matrix(([-1, 0, 0, 0],
                [ 0, 0, 1, 0],
                [ 0, 1, 0, 0],
                [ 0, 0, 0, 1]))
InvMatGL = MatGL.inverted()

def writeTransform(file, obj):
    m = MatGL * obj.matrix_local * InvMatGL
    m.transpose()
    for row in m:
        for value in row:
            file.write(struct.pack('f', value))

def writeString(file, str):
    bstr = bytes(str, 'cp1252')
    file.write(struct.pack('B', len(bstr)))
    for b in bstr:
        file.write(struct.pack('B', b))

def writeMaterial(file, material):
    for i in range(2):
        texName = ''
        slot = material.texture_slots[i]
        if slot != None:
            texName = bpy.path.basename(slot.texture.image.filepath)
        writeString(file, texName)

def writeMesh(file, mesh):
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.triangulate(bm, faces = bm.faces)
    triMesh = bpy.data.meshes.new(name = '~temp~')
    bm.to_mesh(triMesh)
    triMesh.calc_tessface()
    triMesh.calc_tangents()
    bm.free()
    
    indices = []
    dict = {}
    
    uvs = triMesh.tessface_uv_textures
    
    for f in triMesh.tessfaces:
        for i in range(3):
            vIndex = f.vertices[i]
            vertex = triMesh.vertices[vIndex]
            vc = MatGL * vertex.co
            vn = MatGL * vertex.normal
            t0 = uvs[0].data[f.index].uv[i]
            t1 = uvs[1].data[f.index].uv[i]
            
            vp = ( vc.x, vc.y, vc.z,
                    vn.x, vn.y, vn.z,
                    t0[0], 1.0 - t0[1],
                    t1[0], 1.0 - t1[1] )
                    
            index = dict.setdefault(vp, len(dict))
            
            indices.append(index)
    
#    indices = list(range(len(dict)))
    
    bpy.data.meshes.remove(triMesh)
    
    vertices = [None] * len(dict)
    for key in dict.keys():
        vertices[dict.get(key)] = key
    
    file.write(struct.pack('i', len(indices)))
    for i in indices:
        file.write(struct.pack('H', i))
        
    file.write(struct.pack('i', len(vertices)))
    for vp in vertices:
        for v in vp:
            file.write(struct.pack('f', v))

def collect(path):
    print('convert scene')
    file = open(path, 'wb')
    
    for obj in bpy.data.scenes[0].objects:
        print(obj.name)
        type = -1
        if obj.type == 'MESH':
            type = 0
        if obj.type == 'LAMP':
            type = 1
        if obj.name.startswith('#respawn'):
            type = 2
        if type == -1:
            continue
        file.write(struct.pack('i', type))
        writeTransform(file, obj)
        if type == 0:
            writeMaterial(file, obj.material_slots[0].material)
            writeMesh(file, obj.data);

    file.close()
    print('done')
    
collect('C:/Projects/RealTournament/bin/level1.lvl')
