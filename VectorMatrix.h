//Header with Vector and Matrix utility functions
#pragma once
struct vector3D {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
	vector3D operator+(vector3D& b);
	vector3D operator-(vector3D& b);
	vector3D operator*(float k);
	vector3D operator/(float k);
	float vDotProduct(vector3D& b);
	vector3D vCrossProduct(vector3D& b);
	float vLength();
	vector3D vNormalise();
};

struct mat4x4 {
	float m[4][4] = { 0 };
	mat4x4 operator*(mat4x4& b);
	mat4x4 mProject(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
	mat4x4 mRotateX(float fAngleRad);
	mat4x4 mRotateY(float fAngleRad);
	mat4x4 mRotateZ(float fAngleRad);
	mat4x4 mTranslate(float x, float y, float z);
	mat4x4 mIdentity();
	mat4x4 mPointAt(vector3D& pos, vector3D& target, vector3D& up);
	mat4x4 mQuickInverse(mat4x4& m);
};

//Vector Addition
vector3D vector3D :: operator+ (vector3D& b) {
	vector3D vector;
	vector.x = this->x + b.x;
	vector.y = this->y + b.y;
	vector.z = this->z + b.z;
	vector.w = this->w;
	return vector;
};

//Vector Subtraction
vector3D vector3D :: operator- (vector3D& b) {
	vector3D vector;
	vector.x = this->x - b.x;
	vector.y = this->y - b.y;
	vector.z = this->z - b.z;
	vector.w = this->w;
	return vector;
};

//Multiply vector by a scalar
vector3D vector3D :: operator* (float k) {
	vector3D vector;
	vector.x = this->x * k;
	vector.y = this->y * k;
	vector.z = this->z * k;
	vector.w = this->w;
	return vector;
};

//Divide vector by a scalar
vector3D vector3D :: operator/ (float k) {
	vector3D vector;
	vector.x = this->x / k;
	vector.y = this->y / k;
	vector.z = this->z / k;
	vector.w = this->w;
	return vector;
};

//Vector Dot Product
float vector3D :: vDotProduct(vector3D& b) {
	return this->x * b.x + this->y * b.y + this->z * b.z;
};

//Vector Cross Product
vector3D vector3D::vCrossProduct(vector3D& b) {
	vector3D vector;
	vector.x = this->y * b.z - this->z * b.y;
	vector.y = this->z * b.x - this->x * b.z;
	vector.z = this->x * b.y - this->y * b.x;
	vector.w = this->w;
	return vector;
};

//Vector Matrix Product
vector3D vectorMatrixProduct(vector3D& a, mat4x4& m) {
	vector3D v;
	v.x = a.x * m.m[0][0] + a.y * m.m[1][0] + a.z * m.m[2][0] + a.w * m.m[3][0];
	v.y = a.x * m.m[0][1] + a.y * m.m[1][1] + a.z * m.m[2][1] + a.w * m.m[3][1];
	v.z = a.x * m.m[0][2] + a.y * m.m[1][2] + a.z * m.m[2][2] + a.w * m.m[3][2];
	v.w = a.x * m.m[0][3] + a.y * m.m[1][3] + a.z * m.m[2][3] + a.w * m.m[3][3];
	return v;
};

//Vector Magnitude
float vector3D :: vLength() {
	return (sqrtf(this->vDotProduct(*this)));
};

//Vector Normalisation
vector3D vector3D::vNormalise() {
	vector3D vector;
	float l = this->vLength();
	vector = *this / l;
	return vector;
};

//Polygon
struct triangle {
	vector3D p[3];
	sf::Color clr;
};

//Mesh, a collection of polygons
struct mesh {
	std::vector<triangle> tris;

	bool LoadFromObjectFile(std::string sFilename) {
		std::ifstream f(sFilename);
		if (!f.is_open()) {
			return false;
		}
		std::vector<vector3D> verts;
		while (!f.eof()) {
			char line[128]; //assumption buffer
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char identifier; //variable to store the identifier
			//read vertex
			if (line[0] == 'v') {
				vector3D v;
				s >> identifier >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
			//read face
			if (line[0] == 'f') {
				int f[3];
				s >> identifier >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[(int)f[0] - 1], verts[(int)f[1] - 1], verts[(int)f[2] - 1] });
			}
		}
		return true;
	}
};

//Matrix Multiplication
mat4x4 mat4x4 :: operator* (mat4x4& b) {
	mat4x4 matrix;
	for (int c = 0; c < 4; c++) {
		for (int r = 0; r < 4; r++) {
			matrix.m[r][c] = this->m[r][0] * b.m[0][c] + this->m[r][1] * b.m[1][c] + this->m[r][2] * b.m[2][c] + this->m[r][3] * b.m[3][c];
		}
			
	}
	return matrix;
};

//Identity Matrix
mat4x4 mat4x4::mIdentity() {
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

//Rotation Matrix (X-axis)
mat4x4 mat4x4::mRotateX(float fAngleRad) {
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

//Rotation Matrix (Y-axis)
mat4x4 mat4x4::mRotateY(float fAngleRad) {
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

//Rotation Matrix (Z-axis)
mat4x4 mat4x4::mRotateZ(float fAngleRad) {
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

//Projection Matrix
mat4x4 mat4x4::mProject(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

//Translation Matrix
mat4x4 mat4x4::mTranslate(float x, float y, float z) {
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

//PointAt Matrix
mat4x4 mat4x4 :: mPointAt(vector3D& pos, vector3D& target, vector3D& up) {
	//create direction vector
	vector3D newForward = target - pos;
	newForward = newForward.vNormalise();

	vector3D a = newForward * up.vDotProduct(newForward);
	vector3D newUp = a - up;
	newUp = newUp.vNormalise();

	vector3D newRight = newUp.vCrossProduct(newForward);

	mat4x4 matrix;
	matrix.m[0][0] = newRight.x;
	matrix.m[0][1] = newRight.y;
	matrix.m[0][2] = newRight.z;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;
	matrix.m[1][1] = newUp.y;
	matrix.m[1][2] = newUp.z;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;
	matrix.m[2][1] = newForward.y;
	matrix.m[2][2] = newForward.z;
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;
	matrix.m[3][1] = pos.y;
	matrix.m[3][2] = pos.z;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

//Inverse matrix (Used for the Camera View Matrix)
mat4x4 mat4x4::mQuickInverse(mat4x4& m) {
	mat4x4 matrix;
	matrix.m[0][0] = m.m[0][0];
	matrix.m[0][1] = m.m[1][0];
	matrix.m[0][2] = m.m[2][0];
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1];
	matrix.m[1][1] = m.m[1][1];
	matrix.m[1][2] = m.m[2][1];
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2];
	matrix.m[2][1] = m.m[1][2];
	matrix.m[2][2] = m.m[2][2];
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

//Intersection of Vector and Plane
vector3D vectorIntersectsPlane(vector3D& plane_p, vector3D& plane_n, vector3D& lineStart, vector3D& lineEnd)
{
	plane_n = plane_n.vNormalise();
	float plane_d = -(plane_n.vDotProduct(plane_p));
	float ad = lineStart.vDotProduct(plane_n);
	float bd = lineEnd.vDotProduct(plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vector3D lineStartToEnd = lineEnd - lineStart;
	vector3D lineToIntersect = lineStartToEnd * t;
	return lineStart + lineToIntersect;
}

//Clipping Algorithm
int Triangle_ClipAgainstPlane(vector3D plane_p, vector3D plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
	//Normalise the plane
	plane_n = plane_n.vNormalise();

	//Return signed distance from point to plane
	auto dist = [&](vector3D& p)
	{
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.vDotProduct(plane_p));
	};

	vector3D* inside_points[3];  int nInsidePointCount = 0; //Array for points inside the plane (positive distance)
	vector3D* outside_points[3]; int nOutsidePointCount = 0; //Array for points outside the plane (negative distance)

	//Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { 
		inside_points[nInsidePointCount++] = &in_tri.p[0]; 
	}
	else { 
		outside_points[nOutsidePointCount++] = &in_tri.p[0]; 
	}
	if (d1 >= 0) { 
		inside_points[nInsidePointCount++] = &in_tri.p[1]; 
	}
	else { 
		outside_points[nOutsidePointCount++] = &in_tri.p[1]; 
	}
	if (d2 >= 0) { 
		inside_points[nInsidePointCount++] = &in_tri.p[2]; 
	}
	else { 
		outside_points[nOutsidePointCount++] = &in_tri.p[2]; 
	}

	//Depending in the amount of poitns inside the plane, we have several outcomes
	if (nInsidePointCount == 0)
	{
		//All points lie on the outside of plane, so clip whole triangle
		//It ceases to exist
		return 0; //No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		//All points lie on the inside of plane, so do nothing
		//and allow the triangle to simply pass through
		out_tri1 = in_tri;
		return 1; //Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		//Triangle should be clipped. As two points lie outside
		//the plane, the triangle simply becomes a smaller triangle

		//Copy appearance info to new triangle
		out_tri1.clr = in_tri.clr;
		//Uncomment the following line to see the how algorithm functions visually
		//out_tri1.clr = sf::Color::Blue;

		//The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];

		//but the two new points are at the locations where the 
		//original sides of the triangle (lines) intersect with the plane
		out_tri1.p[1] = vectorIntersectsPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.p[2] = vectorIntersectsPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		//Triangle should be clipped. As two points lie inside the plane,
		//the clipped triangle becomes a "quad". Fortunately, we can
		//represent a quad with two new triangles

		//Copy appearance info to new triangles
		out_tri1.clr = in_tri.clr;
		out_tri2.clr = in_tri.clr;
		//Uncomment two following lines to see the how algorithm functions visually
		//out_tri1.clr = sf::Color::Red;
		//out_tri2.clr = sf::Color::Green;

		//The first triangle consists of the two inside points and a new
		//point determined by the location where one side of the triangle
		//intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.p[2] = vectorIntersectsPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		//The second triangle is composed of one of he inside points, a
		//new point determined by the intersection of the other side of the 
		//triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.p[2] = vectorIntersectsPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2; //Return two newly formed triangles which form a quad
	}
}
