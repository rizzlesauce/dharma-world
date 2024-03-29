#!BPY

"""
Name: 'C Header (.h)'
Blender: 244
Group: 'Export'
Tooltip: 'Exports header file for use with OpenGL'
"""
import Blender
from Blender import *
import bpy
import bpy
import os
        
def write_obj(filepath):    
    out = file(filepath, 'w')
    sce = bpy.data.scenes.active
    ob = sce.objects.active
    mesh = Mesh.New()        
    mesh.getFromObject(ob.name)
    #put in world space
    mesh.transform(ob.getMatrix(), True)
    #put in game coordinate system
    mesh.transform(Mathutils.RotationMatrix(-90, 4, "X"), True) 

    editmode = Window.EditMode()
    if editmode: Window.EditMode(0)
    has_quads = False
    for f in mesh.faces:
        if len(f) == 4:
            has_quads = True
            break
    
    if has_quads:
        oldmode = Mesh.Mode()
        Mesh.Mode(Mesh.SelectModes['FACE'])
        
        mesh.sel = True
        tempob = sce.objects.new(mesh)
        mesh.quadToTriangle(0) # more=0 shortest length
        oldmode = Mesh.Mode(oldmode)
        sce.objects.unlink(tempob)
        
        Mesh.Mode(oldmode)
    
    objectname = ob.getData(True)
    #basename = objectname.capitalize()

    out.write('#ifndef %s_h\n' % objectname)
    out.write('#define %s_h\n\n' % objectname)

    out.write('#include "gltypes.h"\n\n\n')
        
    if (mesh.faceUV):
        out.write('static const TEXTURED_VERTEX_DATA_3D %sVertexData[] = {\n' % objectname)
        for face in mesh.faces:
            for (vert, uvert) in zip(face.verts, face.uv):
                out.write('\t{/*v:*/{%f, %f, %f}, ' % (vert.co.x, vert.co.y, vert.co.z) )
                out.write('/*n:*/{%f, %f, %f}, ' % (vert.no.x, vert.no.y, vert.no.z))
                out.write('/*t:*/{%f, %f}' % ( uvert.x, uvert.y ) )
                out.write('},\n')
        out.write('};\n\n')
    elif (mesh.vertexColors):
        out.write('static const COLORED_VERTEX_DATA_3D %sVertexData[] = {\n' % objectname)
        for face in mesh.faces:
            for (vert, color) in zip(face.verts, face.col):
                out.write('\t{/*v:*/{%f, %f, %f}, ' % (vert.co.x, vert.co.y, vert.co.z) )
                out.write('/*n:*/{%f, %f, %f}, ' % (vert.no.x, vert.no.y, vert.no.z))
                out.write('/*c:*/{%f, %f, %f, %f}' % ( color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0) )
                out.write('},\n')
        out.write('};\n\n')
    else:
        out.write
        out.write('static const VERTEX_DATA_3D %sVertexData[] = {\n' % objectname)
        for face in mesh.faces:
            for vert in face.verts:
                out.write('\t{/*v:*/{%f, %f, %f}, ' % (vert.co.x, vert.co.y, vert.co.z) )
                out.write('/*n:*/{%f, %f, %f} ' % (vert.no.x, vert.no.y, vert.no.z))
                out.write('},\n')
        out.write('};\n\n')
    
    if editmode: Window.EditMode(1)
    out.write('#define %sNumberOfVertices\t%i\n' % (objectname, len(mesh.faces) * 3) )

    out.write('// Drawing Code:\n')
    out.write('// glEnableClientState(GL_VERTEX_ARRAY);\n')
    if (mesh.faceUV):
        out.write('// glEnableClientState(GL_TEXTURE_COORD_ARRAY);\n')
    elif (mesh.vertexColors):
        out.write('// glEnableClientState(GL_COLOR_ARRAY);\n')
        out.write('// glEnable(GL_COLOR_MATERIAL)\n')
    out.write('// glEnableClientState(GL_NORMAL_ARRAY);\n')
    out.write('// glVertexPointer(3, GL_FLOAT, sizeof(')
    if (mesh.faceUV):
        out.write('TEXTURED_VERTEX_DATA_3D')
    elif (mesh.vertexColors):
        out.write('COLORED_VERTEX_DATA_3D')
    else:
        out.write('VERTEX_DATA_3D')
    out.write('), &%sVertexData[0].vertex);\n' % objectname)
    out.write('// glNormalPointer(GL_FLOAT, sizeof(')
    if (mesh.faceUV):
        out.write('TEXTURED_VERTEX_DATA_3D')
    elif (mesh.vertexColors):
        out.write('COLORED_VERTEX_DATA_3D')
    else:
        out.write('VERTEX_DATA_3D')
    out.write('), &%sVertexData[0].normal);\n' % objectname)
    if (mesh.faceUV):
        out.write('// glTexCoordPointer(2, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &%sVertexData[0].texCoord);\n' % objectname)
    elif (mesh.vertexColors):
        out.write('// glColorPointer(4, GL_FLOAT, sizeof(COLORED_VERTEX_DATA_3D), &%sVertexData[0].color);\n' % objectname)
    out.write('// glDrawArrays(GL_TRIANGLES, 0, %sNumberOfVertices);\n' % objectname)
    out.write('// glDisableClientState(GL_VERTEX_ARRAY);\n')
    if (mesh.faceUV):
        out.write('// glDisableClientState(GL_TEXTURE_COORD_ARRAY);\n')
    elif (mesh.vertexColors):
        out.write('// glDisableClientState(GL_NORMAL_ARRAY);\n')
        out.write('// glDisable(GL_COLOR_MATERIAL);\n')
    out.write('// glDisableClientState(GL_NORMAL_ARRAY);\n\n\n')

    out.write('#endif\n')
    out.close()


#filename = os.path.splitext(Blender.Get('filename'))[0]
sce = bpy.data.scenes.active
ob = sce.objects.active
#filename = os.path.splitext(ob.getData(True))[0]
filename = ob.getData(True)
Blender.Window.FileSelector(write_obj, "Export", '%s.h' % filename)
