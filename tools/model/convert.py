import math

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
		self.x, self.y, self.z = x, y, z

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

# model mesh
class Mesh(object):
	# list of vertex attributes, all are lists Vector3(uv will only use xy)
	# positions, uvs must be provided in the model
	# normals, tangents and binormals may be calculated
	# [Vector3 ......]
	positions = []
	uvs = []
	normals = []
	tangents = []
	binormals = []

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

	return (normal.normalized(), 
			tangent.normalized(), 
			binormal.normalized())

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
	def loadObj(self, file):
		pass

	def save(self, file):
		dom = minidom.getDOMImplementation().createDocument(None, 'Model', None)

		root = dom.documentElement
		dom.appendChild(root)

		try:
			open(file, 'w+').write(dom.toprettyxml(indent='\t', encoding='utf-8 ≈'))
		except Exception as ex:
			print 'error saving file, exp = ', str(ex)

# tests
if __name__ == '__main__':
	m = Mesh()
	m.positions.append(Vector3(1, 2, 3))
	m.uvs.append(Vector3(1, 2, 3))
	m.normals.append(Vector3(0, 1, 0))

	x = Model()
	x.meshes.append(m)

	x.save('test.xml')