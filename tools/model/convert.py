import math, sys, traceback

# http://stackoverflow.com/questions/2401185/python-vector-class
def AutoFloatProperties(*props):
    '''metaclass'''
    class _AutoFloatProperties(type):
        # Inspired by autoprop (http://www.python.org/download/releases/2.2.3/descrintro)
        def __init__(cls, name, bases, cdict):
            super(_AutoFloatProperties, cls).__init__(name, bases, cdict)
            for attr in props:
                def fget(self, _attr='_'+attr): 
                	return getattr(self, _attr)
                def fset(self, value, _attr='_'+attr): 
                	setattr(self, _attr, float(value))
                setattr(cls, attr, property(fget, fset))
    return _AutoFloatProperties

class Vector3(object):
	__metaclass__ = AutoFloatProperties('x','y','z')

	def __init__(self, x=0, y=0, z=0):
		self.x, self.y, self.z = float(x), float(y), float(z)

	def __repr__(self):
		return '[%.6f, %.6f, %.6f]' % (self.x, self.y, self.z)

	def __str__(self):
		return '%.6f %.6f %.6f' % (self.x, self.y, self.z)

	# operators
	def __add__(self, r):
		return Vector3(self.x + r.x,
					   self.y + r.y,
					   self.z + r.z)

	def __sub__(self, r):
		return Vector3(self.x - r.x,
					   self.y - r.y,
					   self.z - r.z)

	def __mul__(self, r):
		return Vector3(self.x * r,
					   self.y * r,
					   self.z * r)

	def __div__(self, r):
		return Vector3(self.x / r,
					   self.y / r,
					   self.z / r);

	def lengthSq(self):
		return self.x * self.x + self.y * self.y + self.z * self.z

	def length(self):
		return math.sqrt(self.lengthSq())

	def cross(self, r):
		return Vector3(self.y * r.z - self.z * r.y,
					   self.z * r.x - self.x * r.z,
					   self.x * r.y - self.y * r.x)

	def normalized(self):
		length = self.length()
		# cannot be normalized
		if length == 0.0:
			return Vector3()
		return Vector3(self.x / length,
					   self.y / length,
					   self.z / length)

# model material
class Material(object):
	# 
	name = "default"
	diffuse = Vector3()
	ambient = Vector3()
	emit = Vector3()
	specular = Vector3()
	opacity = 1.0
	shininess = 1.0
	specular_power = 0

	# texture map => {string, string}
	# key must be one of
	# 	diffuse
	#	specular
	# 	emit
	# 	normal
	# 	height
	textures = {}

class Vertex(object):
	position = Vector3()
	uv = Vector3()
	normal = Vector3()
	tangent = Vector3()
	binormal = Vector3()
	color = Vector3()

	def __init__(self, position = Vector3(), 
					   uv = Vector3(), 
					   normal = Vector3(), 
					   tangent = Vector3(), 
					   binormal = Vector3(), 
					   color = Vector3()):
		self.position, self.uv, self.normal = position, uv, normal
		self.tangent, self.binormal, self.color = tangent, binormal, color
		pass

class Face(object):
	vertices = []

# model mesh
class Mesh(object):
	# list of vertex attributes, all are lists Vector3(uv will only use xy)
	# positions, uvs must be provided in the model
	# normals, tangents and binormals may be calculated
	# [Vertex ......]
	vertices = []

	name = "default"
	material_id = 0

	# [Vector3, Vector3]
	box = []


# calculate normal, tangent and binormal 
# return a tuple of (normal, tangent, binormal)
def CalculateTangentBinormal(p0, p1, p2, uv0, uv1, uv2):
	e0 = p1 - p0
	e1 = p2 - p0
	normal = e0.cross(e1)

	p = p1 - p0
	q = p2 - p0
	s1 = uv1.x - uv0.x
	t1 = uv1.y - uv0.y
	s2 = uv2.x - uv0.x
	t2 = uv2.y - uv0.y

	tmp = 0.0
	d = s1 * t2 - s2 * t1
	if(math.fabs(d) <= 0.000001):
		tmp = 1.0
	else:
		tmp = 1.0 / d

	tangent = Vector3(t2 * p.x - t1 * q.x,
					  t2 * p.y - t1 * q.y,
					  t2 * p.z - t1 * q.z)

	tangent = tangent * tmp

	binormal = Vector3(s1 * q.x - s2 * p.x,
					   s1 * q.y - s2 * p.y,
					   s1 * q.z - s2 * p.z)

	binormal = binormal * tmp

	return normal.normalized(), tangent.normalized(), binormal.normalized()

def ResolveNTB(v1, v2, v3):
	n, t, b = CalculateTangentBinormal(v1.position, v2.position, v3.position,
									   v1.uv, v2.uv, v3.uv)
	v1.normal = v2.normal = v3.normal = n
	v1.tangent = v2.tangent = v3.tangent = t
	v1.binormal = v2.binormal = v3.binormal = b

import xml.dom.minidom as minidom

# ukn model format
# to do with bones, animations...
# <model>
#	<mesh name="" box="" material_id="">
#		<vertex position="" uv="" tangent="" normal="" binormal=""/>
#		...
#	</mesh>
#	<material diffuse="" specular="" emit="" ambient="" opacity="" specularPower="" shininess="">
#		<texture key="" file=""/>
#		...
#	</material>
# <model>

# model representation
class Model(object):
	# [Mesh, ...]
	meshes = []
	# [Materials, ...]
	materials = []

	name = 'Model'

	#[Vector3, Vector3]
	box = []

	# ply - stanford polygon format
	def loadPly(self, file):
		pass

	# meshml used by KlayGE
	def loadKlay(self, file):
		pass

	# collada 
	def loadCollada(self, file):
		pass

	# obj

	def __loadObjMtl(self, f):
		try:
			mtllib_lines = open(f, 'r').readlines()
			current_mat = None

			for mat_line in mtllib_lines:
				mat_cmmds = [x.strip() for x in mat_line.split(' ') ]

				if mat_cmmds[0] == 'newmtl':
					if current_mat != None:
						self.materials.append(current_mat)

					current_mat = Material()
					current_mat.name = mat_cmmds[1]
				else:
					mat_data = [x for x in mat_cmmds[1:] if len(x) > 0]
					if mat_cmmds[0] == 'Ka':
						current_mat.ambient = Vector3(mat_data[0], mat_data[1], mat_data[2])
					elif mat_cmmds[0] == 'Kd':
						current_mat.diffuse = Vector3(mat_data[0], mat_data[1], mat_data[2])
					elif mat_cmmds[0] == 'Ks':
						current_mat.specular = Vector3(mat_data[0], mat_data[1], mat_data[2])
					elif mat_cmmds[0] == 'Ns':
						current_mat.specular_power = float(mat_data[0])
					elif mat_cmmds[0] == 'd' or mat_cmmds[0] == 'Tr':
						current_mat.opacity = float(mat_data[0])
					elif mat_cmmds[0] == 'map_Ka':
						current_mat.textures.update({'ambient': mat_data[-1]})
					elif mat_cmmds[0] == 'map_Kd':
						current_mat.textures.update({'diffuse': mat_data[-1]})
					elif mat_cmmds[0] == 'map_Ks':
						current_mat.textures.update({'specular': mat_data[-1]})
					elif mat_cmmds[0] == 'map_bump' or mat_cmmds[0] == 'bump':
						current_mat.textures.update({'normal': mat_data[-1]})

			if current_mat != None:
				self.materials.append(current_mat)

		except Exception as exp:
			print 'error opening mtllib file: ', data[0], ', exp = ', str(exp)

	def loadObj(self, f):
		try:
			OBJ_VERTEX = 1
			OBJ_UV = 2
			OBJ_FACE = 3
			OBJ_VP = 4
			OBJ_NORMAL = 5
			OBJ_MATERIAL = 6
			USE_MATERIAL = 7

			options = {
				'v': OBJ_VERTEX,		# vertices
				'vt': OBJ_UV,			# uv
				'f': OBJ_FACE, 		# faces, must after vertices
				'vp': OBJ_VP,			# param space vertices
				'vn': OBJ_NORMAL, 	# normals
				'mtllib': OBJ_MATERIAL,
				'usemtl': USE_MATERIAL,
			}

			mesh = Mesh()

			lines = []
			try:
				lines = open(f, 'r').readlines()
			except Exception as exp:
				print 'error opening model file ', f, str(exp)

			positions = [Vector3(), ]
			uvs = [Vector3(), ]
			normals = [Vector3(), ]
			mat_id = -1

			for line in lines:
				if line.startswith('#') or len(line) <= 1:
					# comment line
					pass
				else:
					components = [x.strip() for x in line.split(' ') ]
					if len(components) > 0 and not components[0] in options:
						print 'unknown keyword: ', components[0]
						continue;

					select = options[components[0]]
					data = [x for x in components[1:] if len(x) > 0]
					if select == OBJ_VERTEX:
						positions.append(Vector3(data[0], data[1], data[2]))
						pass
					elif select == OBJ_UV:
						uvs.append(Vector3(data[0], data[1], 0))
						pass
					elif select == OBJ_NORMAL:
						normals.append(Vector3(data[0], data[1], data[2]))
						pass
					elif select == OBJ_FACE:
						face_data = [x.split('/') for x in data]
						try:
							# if more than 3 vertices in a line, build triangles
							for i in range(0, len(face_data) - 2):
								print face_data[0]
								# f v1 v2 v3...
								if len(face_data[0]) == 1:
									v1 = Vertex(position = positions[int(face_data[0+i][0])])
									v2 = Vertex(position = positions[int(face_data[1+i][0])])
									v3 = Vertex(position = positions[int(face_data[2+i][0])])

									mesh.vertices += [v1, v2, v3]
								# f v1/vt1 v2/vt2 ....
								elif len(face_data[0]) == 2:
									v1 = Vertex(position = positions[int(face_data[0+i][0])],
												uv = uvs[int(face_data[0+i][1])])
									v2 = Vertex(position = positions[int(face_data[1+i][0])],
												uv = uvs[int(face_data[1+i][1])])
									v3 = Vertex(position = positions[int(face_data[2+i][0])],
												uv = uvs[int(face_data[2+i][1])])
									mesh.vertices += [v1, v2, v3]
								# f v1/vt1/vn1 v2/vt2/vn2 ... or v1//vn1 v2//vn2
								elif len(face_data[0]) == 3:
									if len(face_data[0][1]) != '0':
										v1 = Vertex(position = positions[int(face_data[0+i][0])],
													uv = uvs[int(face_data[0+i][1])],
													normal = normals[int(face_data[0+i][2])])
										v2 = Vertex(position = positions[int(face_data[1+i][0])],
													uv = uvs[int(face_data[1+i][1])],
													normal = normals[int(face_data[1+i][2])])
										v3 = Vertex(position = positions[int(face_data[2+i][0])],
													uv = uvs[int(face_data[2+i][1])],
													normal = normals[int(face_data[2+i][2])])
										mesh.vertices += [v1, v2, v3]
									else:								
										# v1//vn1 v2//vn2
										v1 = Vertex(position = positions[int(face_data[0+i][0])],
													normal = normals[int(face_data[0+i][2])])
										v2 = Vertex(position = positions[int(face_data[1+i][0])],
													normal = normals[int(face_data[1+i][2])])
										v3 = Vertex(position = positions[int(face_data[2+i][0])],
													normal = normals[int(face_data[2+i][2])])
										mesh.vertices += [v1, v2, v3]
								else:
									print 'unrecognized face: ', face_data
						except Exception as exp:
							import traceback

							print str(exp)
							print traceback.format_exc()

						pass
					elif select == OBJ_VP:
						# not supported currently
						pass

					elif select == OBJ_MATERIAL:
						print '* found material lib: ', data[0]
						self.__loadObjMtl(data[0])

					elif select == USE_MATERIAL:
						print '* selecting material'
						for i in range(0, len(self.materials)):
							print self.materials[i].name
							if self.materials[i].name == data[0]:
								mat_id = i
								print '* using material ', data[0], ', id = ', mat_id
								break
						pass

			i = 0
			while i < len(mesh.vertices):
				ResolveNTB(mesh.vertices[i], mesh.vertices[i+1], mesh.vertices[i+2])
				i += 3

			self.meshes.append(mesh)

		except Exception as err:
			print 'Error while loading file ', f, ' exception = ', str(err) 
			traceback.print_exc(file=sys.stdout)

	SAVE_NORMAL = 1
	SAVE_TANGENT = 1 << 1
	SAVE_BINORMAL = 1 << 2
	SAVE_UV = 1 << 3

	def save(self, file, options = SAVE_NORMAL | SAVE_UV):
		dom = minidom.getDOMImplementation().createDocument(None, 'model', None)

		root = dom.documentElement
		dom.appendChild(root)

		for material in self.materials:
			materialElement = dom.createElement('material')
			materialElement.setAttribute('name', 			str(material.name))
			materialElement.setAttribute('ambient', 		str(material.ambient))
			materialElement.setAttribute('diffuse', 		str(material.diffuse))
			materialElement.setAttribute('specular_power', 	str(material.specular_power))
			materialElement.setAttribute('specular', 		str(material.specular))
			materialElement.setAttribute('shininess', 		str(material.shininess))
			materialElement.setAttribute('opacity', 		str(material.opacity))
			materialElement.setAttribute('emit', 			str(material.emit))
			
			try:
				for (tkey, tname) in material.textures:
					textureElement = dom.createElement('texture')
					textureElement.setAttribute('key', tkey)
					textureElement.setAttribute('file', tname)
					materialElement.appendChild(textureElement)
			except:
				import traceback
				print traceback.format_exc()
				print material.textures

			root.appendChild(materialElement)

		for mesh in self.meshes:
			meshElement = dom.createElement('mesh')
			meshElement.setAttribute('name', mesh.name)
			meshElement.setAttribute('material_id', str(mesh.material_id))
			for vertex in mesh.vertices:
				vertexElement = dom.createElement('vertex')
				meshElement.appendChild(vertexElement)

				vertexElement.setAttribute('position', str(vertex.position))
				if options & self.SAVE_NORMAL:
					vertexElement.setAttribute('normal', str(vertex.normal))
				if options & self.SAVE_UV:
					vertexElement.setAttribute('uv', str(vertex.uv))
				if options & self.SAVE_TANGENT:
					vertexElement.setAttribute('tangent', str(vertex.tangent))
				if options & self.SAVE_BINORMAL:
					vertexElement.setAttribute('binormal', str(vertex.binormal))
				
			root.appendChild(meshElement)
			pass

		try:
			open(file, 'w+').write(dom.toprettyxml(indent='\t', encoding='utf-8'))
		except Exception as ex:
			print 'error saving file, exp = ', str(ex)

# tests
if __name__ == '__main__':
    print sys.argv
    if len(sys.argv) < 3:
        x = Model()
        x.loadObj('dragon_test.obj')
        x.save('test.xml')
    else:
        if len(sys.argv) >= 3:
            x = Model()
            x.loadObj(sys.argv[1])
            x.save(sys.argv[2])

