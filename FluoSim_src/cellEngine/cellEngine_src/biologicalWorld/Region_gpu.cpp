
/*
 *  FluoSim (simulation program of live cell fluorescence microscopy experiments)
 *  Copyright (C) 2020 Matthieu Lagardère
 *
 *  FluoSim is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FluoSim is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License has been attached
 *  along with this program. It can also be found at <https://www.gnu.org/licenses/>.
 *
*/



#include "Region_gpu.h"

#include "toolBox_src/toolbox_library_global.h"
	#include "toolBox_src/fileAndstring_manipulation/file_manipulation.h"
	#include "toolBox_src/fileAndstring_manipulation/string_manipulation.h"


using namespace std;
using namespace glm;


Region::Region(myMultiVector<glm::vec2>& r_mv, gstd::gMultiVector<glm::vec2>& r_gmv) :

	myGLObject(myGLObject::RENDERABLE_ABILITY_BIT, myGLObject::UNDEFINED_TYPE),
	m_r(r_mv.addSubVector()),
	m_r_gv(r_gmv.addSubVector())

{
	m_region_radiusSquared =-1.0;
	m_surface = -1.0;
	m_color = vec4(0.0,0.0,1.0,1.0);
    m_highlighted_color = vec4(0,1,0,1);
    m_isHighlighted = false;

	string vs_raw_src;
	vs_raw_src +="#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"uniform mat4 scaling_matrix;\n"
//				"uniform mat4 screen_matrix;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = scaling_matrix*temp;\n"
				"}\n";

	string fs_raw_src;
	fs_raw_src +="#version 150 core\n\n"
				"\n"
				"uniform vec4 color;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor=color;\n"
				"}\n";

	string log;
	gstd::gShader vs_raw(gstd::VERTEX_SHADER);
		vs_raw.setSource(vs_raw_src, 0);
		vs_raw.compile(log);
	gstd::gShader fs_raw(gstd::FRAGMENT_SHADER);
		fs_raw.setSource(fs_raw_src, 0);
		fs_raw.compile(log);
	gstd::gProgram rawPrimitiveRendering_pgm("rawRendering");
		rawPrimitiveRendering_pgm.addShader(vs_raw);
		rawPrimitiveRendering_pgm.addShader(fs_raw);
		rawPrimitiveRendering_pgm.linkShaders();

	myGLObject::addProgram(rawPrimitiveRendering_pgm);
}

Region::Region(myMultiVector<glm::vec2>& r_mv, gstd::gMultiVector<glm::vec2>& r_gmv,
				   vector<vec2>& r) :

	myGLObject(myGLObject::RENDERABLE_ABILITY_BIT, myGLObject::UNDEFINED_TYPE),
	m_r(r_mv.addSubVector()),
	m_r_gv(r_gmv.addSubVector())

{
	m_r.insert(0,r);
	m_r_gv.insert(0, r);

	m_color = vec4(0.0,0.0,1.0,1.0);
    m_highlighted_color = vec4(0,1,0,1);
    m_isHighlighted = false;

	computeBarycenter();
	computeRadiusSquared();
	m_surface = -1.0;

	string vs_raw_src;
	vs_raw_src +="#version 150 core\n"
				"\n"
				"in vec2 R;\n"
				"uniform mat4 scaling_matrix;\n"
//				"uniform mat4 screen_matrix;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	vec4 temp = vec4(R,0.0, 1.0);\n"
				"	gl_Position = scaling_matrix*temp;\n"
				"}\n";

	string fs_raw_src;
	fs_raw_src +="#version 150 core\n\n"
				"\n"
				"uniform vec4 color;\n"
				"out vec4 gl_FragColor;\n"
				"\n"
				"void main(void)\n"
				"{\n"
				"	gl_FragColor=color;\n"
				"}\n";

	string log;
	gstd::gShader vs_raw(gstd::VERTEX_SHADER);
		vs_raw.setSource(vs_raw_src, 0);
		vs_raw.compile(log);
	gstd::gShader fs_raw(gstd::FRAGMENT_SHADER);
		fs_raw.setSource(fs_raw_src, 0);
		fs_raw.compile(log);
	gstd::gProgram rawPrimitiveRendering_pgm("rawRendering");
		rawPrimitiveRendering_pgm.addShader(vs_raw);
		rawPrimitiveRendering_pgm.addShader(fs_raw);
		rawPrimitiveRendering_pgm.linkShaders();

	myGLObject::addProgram(rawPrimitiveRendering_pgm);

}

bool Region::operator==(Region& rgn)
{

	if(rgn.m_r.size() != m_r.size()) return false; //->

	bool haveSameVertices = true;
	for(uint pt_idx = 0; pt_idx < m_r.size(); pt_idx++)
	{
		vec2 r1 = m_r[pt_idx];
		vec2 r2 = rgn.m_r[pt_idx];

		if(r1.x != r2.x || r1.y != r2.y)
		{
			haveSameVertices = false;
			break;
		}
	}

	return haveSameVertices;


}


void Region::render(myGLObject* renderingTarget)
{
    if(renderingTarget->isRenderingTargetObject() == false || isBlinking()&&isBlinked()) return; //->

	mat4 m;
	m = renderingTarget->getWorldToExtendedHomMatrix();
    vec4 color = m_isHighlighted ? m_highlighted_color : m_color;

	gstd::gProgram& pgm = myGLObject::getProgram(0);

	glDisable(GL_BLEND);
	glLineWidth(2.0);

	pgm.useProgram(true);

	gstd::myConnector<vec2>::connect(pgm, "R", m_r_gv);
	gstd::connectUniform(pgm, "scaling_matrix", (float*) &m);
//	gstd::connectUniform(pgm, "screen_matrix", (float*) &screen_matrix);
    gstd::connectUniform(pgm, "color", &color);

	int n_vertices_segment = m_r.size();
	glDrawArrays(GL_LINE_LOOP, 0, n_vertices_segment);

	pgm.useProgram(false);
}



void Region::setName(const string& name)
{
	m_name = name;
}

string Region::getName() const
{
	return m_name;
}

Region::~Region()
{
}

void Region::addPoint(vec2& r)
{
	m_r.push_back(r);
	m_r_gv.push_back(r);

}

void Region::addPoints(std::vector<glm::vec2>& r_v)
{
	m_r.insert(m_r.size(), r_v);
	m_r_gv.insert(m_r_gv.size(), r_v);

}

const vec2 Region::getPoint(const int pt_idx)
{
	if(pt_idx >= getSize())
	{
	   return vec2(-1,-1);
	}

	const glm::vec2 r = m_r[pt_idx];
	return r;
}

void Region::clearRegion()
{
	m_r.clear();
	m_r_gv.clear();
}

vec4 Region::getColor() const
{
	return m_color;
}

vec4 Region::getHighlightedColor() const
{
    return m_highlighted_color;
}

bool Region::isHighlighted() const
{
    return m_isHighlighted;
}

void Region::setColor(const vec4& color)
{
	m_color = color;
}

void Region::setHighlightedColor(const vec4& color)
{
    m_highlighted_color = color;
}

void Region::setIsHighlighted(bool isHighlighted)
{
    m_isHighlighted = isHighlighted;
}

int Region::getSize() const
{
	return m_r.size();
}

bool Region::getRegionSubData(vector<vec2> *region_points, const int offset, const int n_sub_data) const
{
	if(offset+n_sub_data<=m_r.size()) //c'est bon
	{
		region_points->insert(region_points->begin(), m_r.begin() + offset, m_r.begin() + offset + n_sub_data);
		return 1;
	}
	else //out of scope
	{
		return 0;
	}
}

bool Region::isInside(const vec2& r) const
{
	if(std::pow((r - m_barycenter_r).x, 2.0) + std::pow((r - m_barycenter_r).y, 2.0) >= m_region_radiusSquared )
	{
		return 0;
	}

	bool in = false;
	int N = m_r.size();
	vec2 vec_u(0.5f,0.5f);

	int prev_pt_idx = N-1;
	for(int pt_idx=0; pt_idx<=N-1; pt_idx++)
	{
		vec2 vec_t = m_r[pt_idx] - m_r[prev_pt_idx];
		vec2 delta = r - m_r[prev_pt_idx];
		float D = vec_t[1]*vec_u[0]-vec_t[0]*vec_u[1];

		if(D!=0)
		{
			float t=(delta[1]*vec_u[0]-delta[0]*vec_u[1])/D; //along the curve line
			float t_bis=(delta[1]*vec_t[0]-delta[0]*vec_t[1])/D;

			if(t>=0 && t<1 && t_bis>=0) in = !in;
		}
		prev_pt_idx = pt_idx;
	}

	return in;
}


float Region::getMaximumRadiusSquared(const glm::vec2& r, bool& isInsideRgn) const
{
	isInsideRgn = isInside(r);

	int N = m_r.size();
	float temp_radiusSquared = -1;
	float radiusSquared = std::numeric_limits<float>::max();

	int prev_pt_idx = N-1;
	for(int pt_idx=0; pt_idx<=N-1; pt_idx++)
	{
		vec2 vec_t = m_r[pt_idx] - m_r[prev_pt_idx];
		vec2 vec_n(-vec_t.y, +vec_t.x);
		vec2 delta = r - m_r[prev_pt_idx];

		float D=vec_t[1]*vec_n[0]-vec_t[0]*vec_n[1];
		float t, t_bis;
		if(D!=0)
		{
			t = (delta[1]*vec_n[0]-delta[0]*vec_n[1])/D; //along the curve line
			t_bis = (delta[1]*vec_t[0]-delta[0]*vec_t[1])/D;
		}

		if(t < 0) temp_radiusSquared = delta.x*delta.x + delta.y*delta.y;
		else
		{
			if(t > 1)
			{
				temp_radiusSquared = (-delta + vec_t).x*(-delta + vec_t).x + (-delta + vec_t).y*(-delta + vec_t).y;
			}
            else //t >= 0 && t<= 1 i.e.: we are outside of the segment
			{
				temp_radiusSquared = (-delta + t*vec_t).x*(-delta + t*vec_t).x + (-delta + t*vec_t).y*(-delta + t*vec_t).y;
			}
		}

		if(temp_radiusSquared <= radiusSquared) radiusSquared = temp_radiusSquared;

		prev_pt_idx = pt_idx;
	}

	return radiusSquared;
}


//Map setting
void Region::addDynamicParam(ChemicalSpecies* spc)
{
	myDynamicParam dyn_param;
	m_dynamicParams_map.insert({spc, dyn_param});
}

void Region::removeDynamicParam(ChemicalSpecies* spc)
{
	auto dyn_map = m_dynamicParams_map.find(spc);
	m_dynamicParams_map.erase(dyn_map);
}

bool Region::isACompartment(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).isACompartment;
}


//individual setting
void Region::setIsACompartment(ChemicalSpecies* spc, bool new_state)
{
	m_dynamicParams_map[spc].isACompartment = new_state;
}

void Region::setIsDDistrubuted(ChemicalSpecies* spc, bool isDDistributed)
{
	m_dynamicParams_map[spc].isDDistributed = isDDistributed;
}

void Region::setIsTrappingEnable(ChemicalSpecies* spc, bool isTrapping_enable)
{
	m_dynamicParams_map[spc].isTrapping_enable = isTrapping_enable;
}

void Region::setAreSitesAbundant(ChemicalSpecies *spc, bool areSites_abundant)
{
	m_dynamicParams_map[spc].areSites_abundant = areSites_abundant;
}

void Region::setD(ChemicalSpecies* spc, float D)
{
	m_dynamicParams_map[spc].D = D;
}

void Region::setDtrapped(ChemicalSpecies* spc, float Dtrapped)
{
	m_dynamicParams_map[spc].Dt = Dtrapped;
}

void Region::setKonAbundant(ChemicalSpecies *spc, float kt_on_abundant)
{
	m_dynamicParams_map[spc].kt_on_abundant = kt_on_abundant;
}

void Region::setKonNAbundant(ChemicalSpecies *spc, float kt_on_Nabundant)
{
	m_dynamicParams_map[spc].kt_on_Nabundant = kt_on_Nabundant;
}

void Region::setSiteDensity(ChemicalSpecies* spc, float site_density)
{
	m_dynamicParams_map[spc].site_density = site_density;;
}


void Region::setKoff(ChemicalSpecies* spc, float kt_off)
{
	m_dynamicParams_map[spc].kt_off = kt_off;
}

void Region::setNumberTrappedPrtl(ChemicalSpecies* spc, int nb_ptcl)
{
	m_dynamicParams_map[spc].nb_trappedPtcl = nb_ptcl;
}

void Region::addTrappedPrtlToNb(ChemicalSpecies *spc)
{
	m_dynamicParams_map[spc].nb_trappedPtcl++;
}

void Region::deleteATrappedPrtlToNb(ChemicalSpecies *spc)
{
	m_dynamicParams_map[spc].nb_trappedPtcl--;
}

void Region::setPoissonMean(ChemicalSpecies* spc, float poisson_mean)
{
	m_dynamicParams_map[spc].poisson_mean = poisson_mean;
}

void Region::setCrossing(ChemicalSpecies* spc, float p_crossing_insideOut, float p_crossing_outsideIn)
{
	m_dynamicParams_map[spc].p_crossing_insideOut = p_crossing_insideOut;
	m_dynamicParams_map[spc].p_crossing_outsideIn = p_crossing_outsideIn;
}


//individual getting
bool Region::isTrappingActivated(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).isTrapping_enable;
}

bool Region::isDDistributed(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).isDDistributed;
}

bool Region::areSitesAbundant(ChemicalSpecies *spc) const
{
	return m_dynamicParams_map.at(spc).areSites_abundant;
}

float Region::getDtrapped(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).Dt;
}

float Region::getD(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).D;
}

float Region::getKonAbundant(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).kt_on_abundant;
}

float Region::getKonNAbundant(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).kt_on_Nabundant;
}

float Region::getSiteDensity(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).site_density;
}

float Region::getKoff(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).kt_off;
}

float Region::getCurrentSiteDensity(ChemicalSpecies* spc) const
{
	int nb_trapped_ptcl = m_dynamicParams_map.at(spc).nb_trappedPtcl;
	float site_density = m_dynamicParams_map.at(spc).site_density;

	return 1.0 - (float(nb_trapped_ptcl)/(getSurface()*site_density));
}

int Region::getNumberTrappedPrtl(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).nb_trappedPtcl;
}

float Region::getPoissonMean(ChemicalSpecies* spc) const
{
	return m_dynamicParams_map.at(spc).poisson_mean;
}

float Region::getCrossing(ChemicalSpecies* spc, CROSSING_DIRECTION cross_dir) const
{
	if(cross_dir == INOUT_CROSSING)	return m_dynamicParams_map.at(spc).p_crossing_insideOut;
	else return m_dynamicParams_map.at(spc).p_crossing_outsideIn;

}



void Region::computeBarycenter()
{
	if(getSize() == 0)
	{
        cout<<"Error : in Region::computeBarycenter() : getSize() == 0\n";
		return;
	}

	vec2 barycenter(0.0,0.0);

	for(int r_idx = 0; r_idx <=getSize()-1; r_idx++)
	{
		barycenter = barycenter + m_r[r_idx];
	}

	barycenter = 1/float(getSize())* barycenter;

	m_barycenter_r = barycenter;
}

void Region::computeRadiusSquared()
{
	if(getSize() == 0)
	{
		cout<<"Error : in myRegion::computeRadius() : getSize() == 0\n";
		return;
	}

	int r_idx_max;
	float distance_squared_max=-1;
	float distance_squared;



	for(int r_idx = 0; r_idx <=getSize()-1; r_idx++)
	{
		distance_squared =  std::pow((m_r[r_idx] - m_barycenter_r).x, 2.0) +
							std::pow((m_r[r_idx] - m_barycenter_r).y, 2.0);

		if(distance_squared_max == -1)
		{
			r_idx_max = 0;
			distance_squared_max = distance_squared;
		}
		else
		{
			if(distance_squared >= distance_squared_max)
			{
				r_idx_max = r_idx;
				distance_squared_max = distance_squared;
			}
		}
	}

	m_region_radiusSquared = distance_squared_max;
}

void Region::computeSurface()
{
	float A = 0.0f;
	int prev_pt_idx = getSize()-1;
	for(int pt_idx = 0; pt_idx<=getSize()-1; pt_idx++)
	{
		A += m_r[prev_pt_idx].x * m_r[pt_idx].y - m_r[pt_idx].x * m_r[prev_pt_idx].y;
		prev_pt_idx = pt_idx;
	}
	m_surface = abs(0.5*A);
}


vec2 Region::getBarycenter() const
{
	return m_barycenter_r;
}


float Region::getRadiusSquared() const
{

//    if(m_region_radiusSquared ==-1) computeRadiusSquared();

	return m_region_radiusSquared;
}

float Region::getSurface() const
{
	return m_surface;
}

bool Region::getBottomLeft(glm::vec2 &bottom_left) const
{
	if(m_r.size() == 0) return false; //->

	bottom_left = m_r[0];
	for(vec2 r : m_r)
	{
		if(r.x <= bottom_left.x)
		{
		   bottom_left.x = r.x;
		}

		if(r.y <= bottom_left.y)
		{
			bottom_left.y = r.y;
		}
	}

	return true;
}

bool Region::getTopRight(glm::vec2 &top_right) const
{
	if(m_r.size() == 0) return false; //->

	top_right = m_r[0];
	for(vec2 r : m_r)
	{
		if(r.x >= top_right.x)
		{
			top_right.x = r.x;
		}

		if(r.y >= top_right.y)
		{
			top_right.y = r.y;
		}
	}

	return true;

}


void Region::scaleRegion(float scaling_param)
{
	if(m_r.size() ==0)
	{
        cout<<"In Region::scaleRegion(float) : m_r.size() ==0\n";
		return;
	}

	computeBarycenter();

	for(int pt_idx = 0; pt_idx <= m_r.size()-1; pt_idx++)
	{
		m_r[pt_idx] = m_barycenter_r + scaling_param * (m_r[pt_idx] - m_barycenter_r);
	}

	computeRadiusSquared();
	computeSurface();
}

void Region::reflect(vec2 r_start, vec2 dr_start, int edge_to_avoid_start,
					   vec2 &r_end, vec2 &dr_end, int &edge_to_avoid_end,
					   RandomNumberGenerator& randomFactory, float p_crossing)
{
	int N = m_r.size();

	vec2 delta_R;

	vec2 vec_u = dr_start;
	vec2 vec_t;
	float D;
	float t, u;
	float u_min;
	int i_min;

	u_min = std::numeric_limits<float>::max();
	i_min=-1;

    for(int i=0; i<N; i++) //the first element tells the number of elements
	{
		if(i==edge_to_avoid_start) continue; //<-

		if(i != N-1) vec_t = m_r[i+1] - m_r[i];
		else vec_t = m_r[0] - m_r[i];

		delta_R = r_start - m_r[i];
		D=vec_t.y*vec_u.x-vec_t.x*vec_u.y;

		if(D == 0.0)
		{
			continue; //<-
		}

		t=(delta_R.y*vec_u.x-delta_R.x*vec_u.y)/D; //along the curve line
		u=(delta_R.y*vec_t.x-delta_R.x*vec_t.y)/D; //along the direction u

		if(t>0.0 && t<1.0 && u>0.0 && u<1.0) //t_bis has to be different from 0, or the point wont't move!
		{
			if(u<u_min)
			{
				u_min=u;
				i_min=i;
			}
		}
	}

    if(i_min == -1) //no intersection
	{
		r_end = r_start + (1.0f-0.20f)*vec_u;
		dr_end = vec2(0,0);
		edge_to_avoid_end = -1;
	}

	else
	{
		if(randomFactory.uniformRandomNumber() <= p_crossing) //crossing
		{
			u = (1+(0.000))* u_min;
			r_end = r_start + u*vec_u;

			dr_end = (1.0f-u_min)*vec_u;
			edge_to_avoid_end = i_min;
		}
		else //no crossing
		{
			u = (1.0f-0.20f)* u_min;
			r_end = r_start + u*vec_u;

			if(i_min != N-1)  vec_t = m_r[i_min+1] - m_r[i_min];
			else vec_t = m_r[0] - m_r[i_min];

			vec2 dir_t;
			dir_t = normalize(vec_t);

			float scal = (1-u)*vec_u.x * dir_t.x + (1-u)*vec_u.y*dir_t.y;
			dr_end = 2*scal*dir_t - (1-u)*vec_u;
			edge_to_avoid_end = i_min;
		}
	}
}

bool Region::intersect(glm::vec2 r, glm::vec2 dr, int edge_to_avoid,
						 float& u_end, int& intersected_edge_idx, CROSSING_DIRECTION& cross_dir)
{
	int N = m_r.size();
	uint N_intersection = 0;

	vec2 delta_R;

	uint isOnAnEdge = 0;
	vec2 vec_t, vec_u(dr);
	float D;
	float t, u;
	float u_min = 1.0f;
	int i_min(-1);
	intersected_edge_idx = -1;

	for(int i=0; i<N; i++)
	{
			if(i != N-1) vec_t = m_r[i+1] - m_r[i];
			else vec_t = m_r[0] - m_r[i];

			delta_R = r - m_r[i];
			D=vec_t.y*vec_u.x-vec_t.x*vec_u.y;

			if(D == 0.0) continue; //<-

			t=(delta_R.y*vec_u.x-delta_R.x*vec_u.y)/D; //along the curve line
			u=(delta_R.y*vec_t.x-delta_R.x*vec_t.y)/D; //along the direction u

			if(t>=0.0 && t<1.0 && u>=0.0) //t_bis has to be different from 0, or the point wont't move!
			{
				N_intersection++;

				if(u == 0) isOnAnEdge = 1;
				else
				{
					if(i!=edge_to_avoid)
					{
						if(u<u_min) //u_min is initialized to one
						{
							u_min=u;
							i_min=i;
						}
					}
				}
			}
	}

	u_end = u_min;
	intersected_edge_idx = i_min;
	cross_dir = CROSSING_DIRECTION(N_intersection%2 + 2*isOnAnEdge) ; //enum CROSSING_DIRECTION {OUTIN_CROSSING, INOUT_CROSSING};

	if(i_min != -1) return true;
	else return false;
}



std::vector<vector<glm::vec2> > getPointsFromMetaMorphRgnFile(string metamorphRgn_file)
{
	enum METAMORPH_REGION_TYPE
	{
		NONE_METAMORPH_REGION_TYPE = 0,
		RECTANGLE_METAMORPH_REGION_TYPE = 1,
		ELIPSE_METAMORPH_REGION_TYPE = 5,
		POLYGON_METAMORPH_REGION_TYPE = 3
	};

	vector<vector<glm::vec2> > rgns_v;
		vector<glm::vec2> temp_points;

	string file_content;
		string line;
		string word;
		int file_pos = 0;
		int line_pos = 0;

	if(getFileContent(metamorphRgn_file, file_content) == false)
	{
		cout<<"Can't open file...\n";
		return rgns_v; //->
	}

	while(getLine(file_content, line, file_pos, file_pos))
	{
		line_pos = 0;
		getWord(line, word, line_pos, line_pos);
		getWord(line, word, line_pos, line_pos);
		METAMORPH_REGION_TYPE region_type = (METAMORPH_REGION_TYPE) stoi(word);

		switch(region_type)
		{
			case POLYGON_METAMORPH_REGION_TYPE :
			{
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				temp_points.clear();
				int nb_points = strtod(word.data(), 0);
				for(int pt_idx = 0; pt_idx <= nb_points -1; pt_idx++)
				{
					getWord(line, word, line_pos, line_pos);
					float x = strtod(word.data(), 0);
					getWord(line, word, line_pos, line_pos);
					float y = strtod(word.data(), 0);
					temp_points.push_back(vec2(x,y));
				}

				rgns_v.push_back(temp_points);
			}
			break;

			case RECTANGLE_METAMORPH_REGION_TYPE :
			{
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				vec2 top_right;
				getWord(line, word, line_pos, line_pos);
				top_right.x = stof(word);
				getWord(line, word, line_pos, line_pos);
				top_right.y = stof(word);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				vec2 diag;
				getWord(line, word, line_pos, line_pos);
				diag.x = stof(word)-1;
				getWord(line, word, line_pos, line_pos);
				diag.y = stof(word)-1;

				temp_points.clear();

				temp_points.push_back(top_right);
				temp_points.push_back(top_right+ vec2(0,diag.y));
				temp_points.push_back(top_right+ diag);
				temp_points.push_back(top_right+ vec2(diag.x,0));

				rgns_v.push_back(temp_points);
			}
			break;

			case ELIPSE_METAMORPH_REGION_TYPE :
			{
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);
				getWord(line, word, line_pos, line_pos);

				temp_points.clear();
				int nb_points = strtod(word.data(), 0);
				for(int pt_idx = 0; pt_idx <= nb_points -1; pt_idx++)
				{
					getWord(line, word, line_pos, line_pos);
					float x = strtod(word.data(), 0);
					getWord(line, word, line_pos, line_pos);
					float y = strtod(word.data(), 0);
					temp_points.push_back(vec2(x,y));
				}

				rgns_v.push_back(temp_points);
			}
			break;

		}


	}
	return rgns_v;
}


uint jShortTocShort(char* jShort)
{
	return 256* ((unsigned char*) jShort)[0] + ((unsigned char*) jShort)[1];
}


std::vector<glm::vec2> getPointsFromImageJRgnFile(std::string imageJRgn_file)
{
	vector<vec2> r_v;

	//file loading...
	ifstream myfile;
	myfile.open(imageJRgn_file, ios_base::binary);
	if(myfile.is_open() == false) return r_v; //->
	myfile.seekg(0, myfile.end);

	int file_length = myfile.tellg();
	char* file_str;
	file_str = new char[file_length +1];

	myfile.seekg(0, myfile.beg);
	myfile.read(file_str, file_length);
	file_str[file_length] = '\0';
    myfile.close();

	//common parameter reading
	int rgn_type_value = jShortTocShort(file_str + 6);
	IMAGEJ_REGION_TYPE rgn_type = NONE_IMAGEJ_REGION_TYPE;
		if(rgn_type_value == 0) rgn_type = POLYGON_IMAGEJ_REGION_TYPE;
		if(rgn_type_value == 256) rgn_type = RECTANGLE_IMAGEJ_REGION_TYPE;
		if(rgn_type_value == 512) rgn_type = ELLIPSE_IMAGEJ_REGION_TYPE;

	int top = jShortTocShort(file_str + 8);
	int bottom = jShortTocShort(file_str + 12);
	int left = jShortTocShort(file_str + 10);
	int right = jShortTocShort(file_str + 14);

	int nb_points = jShortTocShort(file_str + 16);

	//points loading
	switch(rgn_type)
	{
		case POLYGON_IMAGEJ_REGION_TYPE :
		{
			vec2 r;
			for(int point_idx = 0; point_idx <= nb_points-1; point_idx++)
			{
				r.x = left + jShortTocShort(file_str + 64 + point_idx*2);
				r.y = top + jShortTocShort(file_str + 64 + nb_points*2 + point_idx*2);

				r_v.push_back(r);
			}
		}
		break;

		case RECTANGLE_IMAGEJ_REGION_TYPE :
		{
			r_v.push_back({left, top});
			r_v.push_back({right, top});
			r_v.push_back({right, bottom});
			r_v.push_back({left, bottom});
		}
		break;

		case ELLIPSE_IMAGEJ_REGION_TYPE :
		{
			nb_points = 50;

			float a = right - left;
			cout<<"a : "<<a<<"\n";
			float b = bottom - top;
			cout<<"b : "<<b<<"\n";

			float d_teta = 0;
			float teta = 0;
			float r;
			for(int point_idx = 0; point_idx <= nb_points-1; point_idx++)
			{
				d_teta = 2.0*3.14159/nb_points;
				r = std::sqrt(1.0/(pow(cos(teta)/a, 2.0) + pow(sin(teta)/b, 2.0)));

				r_v.push_back({r*cos(teta),
							   r*sin(teta)});

				teta += d_teta;
			}
		}
		break;

		case NONE_IMAGEJ_REGION_TYPE :
		{
		}
	}

    delete file_str;
	return r_v;
}

void saveRegionsList(list<Region>& regions_l, std::string image_filePath, GEOMETRY_FILE_FORMAT format)
{
	ofstream myfile;
	myfile.open(image_filePath);

	if(myfile.is_open() == false || regions_l.size() == 0) return; //->

	switch(format)
	{
		case METAMORPH_GEOMETRY_FILE_FORMAT :
		{
			for(auto& region : regions_l)
			{
				myfile<<"0 3, 1 16711680, 2 0 0, 3 0 0, 4 0, 5 1, 6 ";
				myfile<<region.getSize()<<" ";
				for(auto&r : region.m_r)
				{
					myfile<<r.x<<" "<<r.y<<" ";
				}
				myfile<<",7 1\n";
			}
		}
		break;

		case SVG_FILE_FORMAT :
		{
			myfile<<"<svg width=\"400\" height=\"400\">\n";

			myfile<<" <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"390\" y=\"390\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"390\" y=\"0\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";
			myfile<<" <rect x=\"0\" y=\"390\" width=\"10\" height=\"10\" stroke=\"#000000\" /> \n";


			for(Region& region : regions_l)
			{
				myfile<<" <polygon points=\"";

				for(glm::vec2& r : region.m_r)
				{
					myfile<<r.x<<","<<r.y<<" ";
				}

//				if(region.m_r.size() >= 2)
//				{
//					glm::vec2& r = region.m_r[0];
//					myfile<<r.x<<","<<r.y<<" ";
//				}

				myfile<<"\" ";

				glm::vec4 color = region.getColor();

				char digit[2] = "0";
				char rgn_red[3] = "00", rgn_green[3] = "00", rgn_blue[3] = "00";
				itoa(int(color[0]*255.0f)/16, digit,16);
				rgn_red[0] = digit[0];
				itoa(int(color[0]*255.0f)%16, digit,16);
				rgn_red[1] = digit[0];

				itoa(int(color[1]*255.0f)/16, digit,16);
				rgn_green[0] = digit[0];
				itoa(int(color[1]*255.0f)%16, digit,16);
				rgn_green[1] = digit[0];

				itoa(int(color[2]*255.0f)/16, digit,16);
				rgn_blue[0] = digit[0];
				itoa(int(color[2]*255.0f)%16, digit,16);
				rgn_blue[1] = digit[0];

				myfile<<"stroke=\"#"<<rgn_red<<rgn_green<<rgn_blue<<"\" ";
				myfile<<"/>\n";
			}

			myfile<<"</svg>";
		}
		break;
	}

	myfile.close();
}


bool getIntersectionRegion(Region& rgn1, Region& rgn2, vector<vec2>& vertices)
{
	if(rgn1.getSize() == 0 || rgn2.getSize() == 0) return false; //->

	if(rgn1 == rgn2)
	{
		if(rgn1.m_r.size() >= 3)
		{
			for(auto r = rgn1.m_r.begin(); r != rgn1.m_r.end(); r++)
			{
				vertices.push_back((vec2) *r);
			}
			return true;//->
		}
		else return false;//->
	}

	enum VERTEX_TYPE {OUTSIDE_VERTEX, INSIDE_VERTEX, INTERSECTION_VERTEX, EXTERNAL_VERTEX};
	struct vert_data
	{
		vec2 r;
		VERTEX_TYPE type;
		int edge;
	};

	 vector<vert_data> out_vertices; //rgn1 inter rgn2
	 vector<vert_data> in_vertices; //rgn1

	 for(vec2& r : rgn2.m_r)
	 {
		if(rgn1.isInside(r) == true)
		{
			in_vertices.push_back({r,INSIDE_VERTEX,-1});
		}
		else in_vertices.push_back({r,OUTSIDE_VERTEX,-1});
	 }
	 vec2& r = rgn2.m_r[0];
	 if(rgn1.isInside(r) == true) in_vertices.push_back({r,INSIDE_VERTEX,-1});
	 else in_vertices.push_back({r,OUTSIDE_VERTEX,-1});

	 //we have to stop before the end
	 for(uint vert_idx = 0; vert_idx < in_vertices.size()-1; vert_idx++)
	 {
		 if(in_vertices[vert_idx].type == INSIDE_VERTEX)
		 {
			 out_vertices.push_back(in_vertices[vert_idx]);
		 }

		 vec2 r = in_vertices[vert_idx].r;
		 vec2 dr = in_vertices[vert_idx+1].r - r;
		 float t_end;
		 int intersected_edge = -1;
		 CROSSING_DIRECTION cross_dir;

		//do we interecept
		 while(rgn1.intersect(r, dr, intersected_edge, t_end, intersected_edge, cross_dir) == true)
		 {
			 r = r + t_end*dr;
			 out_vertices.push_back({r,INTERSECTION_VERTEX,intersected_edge});

			 dr = in_vertices[vert_idx+1].r - r;
		 }
	 }

	 //we have to deal with the last edge
	 if(out_vertices.size() == 2)
	 {
		 vector<vert_data>::iterator vert = out_vertices.begin();
		 for(uint vert_idx = 0; vert_idx < out_vertices.size()-1; vert_idx++)
		 {
			 vert++;
			 if(out_vertices[vert_idx].type == INTERSECTION_VERTEX &&
				out_vertices[(vert_idx+1)%out_vertices.size()].type == INTERSECTION_VERTEX)
			 {
				 if(out_vertices[vert_idx].edge != out_vertices[vert_idx+1].edge)
				 {
					 int edge1 = out_vertices[vert_idx].edge;
					 int edge2 = out_vertices[(vert_idx+1)%out_vertices.size()].edge;

					 bool isFirstDirection = true;
					 int N = rgn1.getSize();
					 int edge = edge1;
					 while(edge != edge2 &&  isFirstDirection == true) //we considere [edge1; edge2[
					 {
						 isFirstDirection = isFirstDirection && rgn2.isInside(rgn1.m_r[(edge+1)%N]);
						 edge = (edge+1)%N;
					 }

					 std::swap(edge1, edge2);
					 bool isSecondDirection = true;
					 N = rgn1.getSize();
					 edge = edge1;
					 while(edge != edge2 &&  isSecondDirection == true) //we considere [edge1; edge2[
					 {
						 isSecondDirection = isSecondDirection && rgn2.isInside(rgn1.m_r[(edge+1)%N]);
						 edge = (edge+1)%N;
					 }

					 if(isFirstDirection == true || isSecondDirection == true)
					 {
						 if(isFirstDirection == true)  std::swap(edge1, edge2);
						 vector<vert_data> temp;

						 N = rgn1.getSize();
						 edge = edge1;
						 while(edge != edge2) //we considere [edge1; edge2[
						 {
							 temp.push_back({rgn1.m_r[(edge+1)%N],EXTERNAL_VERTEX,-1});
							 edge = (edge+1)%N;
						 }

						 if(isSecondDirection == true)
						 {
							 out_vertices.insert(vert, temp.rbegin(), temp.rend());
						 }
						 else out_vertices.insert(vert, temp.begin(), temp.end());
					 }
				 }
			 }
		 }
	 }
	 else
	 {
		 vector<vert_data>::iterator vert;
		 for(int vert_idx = 0; vert_idx < out_vertices.size(); vert_idx++)
		 {
			 vert = out_vertices.begin();
			 advance(vert, vert_idx+1);

			 if(out_vertices[vert_idx].type == INTERSECTION_VERTEX &&
				out_vertices[(vert_idx+1)%out_vertices.size()].type == INTERSECTION_VERTEX)
			 {
				 if(out_vertices[vert_idx].edge != out_vertices[vert_idx+1].edge)
				 {
					 int edge1 = out_vertices[vert_idx].edge;
					 int edge2 = out_vertices[(vert_idx+1)%out_vertices.size()].edge;

					 bool isFirstDirection = true;
					 int N = rgn1.getSize();
					 int edge = edge1;
					 while(edge != edge2 &&  isFirstDirection == true) //we considere [edge1; edge2[
					 {
						 isFirstDirection = isFirstDirection && rgn2.isInside(rgn1.m_r[(edge+1)%N]);
						 edge = (edge+1)%N;
					 }

					 std::swap(edge1, edge2);
					 bool isSecondDirection = true;
					 N = rgn1.getSize();
					 edge = edge1;
					 while(edge != edge2 &&  isSecondDirection == true) //we considere [edge1; edge2[
					 {
						 isSecondDirection = isSecondDirection && rgn2.isInside(rgn1.m_r[(edge+1)%N]);
						 edge = (edge+1)%N;
					 }

					 if(isFirstDirection == true || isSecondDirection == true)
					 {
						 if(isFirstDirection == true)  std::swap(edge1, edge2);
						 vector<vert_data> temp;

						 N = rgn1.getSize();
						 edge = edge1;
						 while(edge != edge2) //we considere [edge1; edge2[
						 {
							 temp.push_back({rgn1.m_r[(edge+1)%N],EXTERNAL_VERTEX,-1});
							 edge = (edge+1)%N;
						 }
						 if(isSecondDirection == true)
						 {
							 out_vertices.insert(vert, temp.rbegin(), temp.rend());
						 }
						 else out_vertices.insert(vert, temp.begin(), temp.end());
					 }
				 }
			 }
		 }
	 }

	for(vert_data vert : out_vertices)
	{
		vertices.push_back(vert.r);
	}

	if(vertices.size() <=2) return false;
	else return true;
}

float computeSurface(vector<glm::vec2>& r_v)
{
	float A = 0.0f;

	int N = r_v.size();
	if(N <= 2) return A;


	int prev_pt_idx = N-1;
	for(int pt_idx = 0; pt_idx<=N-1; pt_idx++)
	{
		A += r_v[prev_pt_idx].x * r_v[pt_idx].y - r_v[pt_idx].x * r_v[prev_pt_idx].y;
		prev_pt_idx = pt_idx;
	}
	return abs(0.5*A);
}


