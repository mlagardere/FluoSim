
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

#ifndef REGION_GPU_H
#define REGION_GPU_H

#include "stdlib.h"
#include "string"
#include "vector"
#include "map"
#include "iostream"
#include "fstream"
#include "iostream"

#include "glm.hpp"
#define GLM_SWIZZLE

#include "cellEngine_library_global.h"
    #include "ChemicalSpecies.h"
    #include "physicsEngine/RandomNumberGenerator.h"

#include "gpuTools_library_global.h"
    #include "gl/gVector.h"
    #include "gl/gMultiVector.h"
    #include "gl/gToolBox.h"

#include "toolBox_src/toolBox_library_global.h"
    #include "toolBox_src/containers/myMultiVector.h"
    #include "toolBox_src/otherFunctions/otherFunctions.h"
    #include "glGUI/glObjects/myGLObject.h"

enum GEOMETRY_FILE_FORMAT {IMAGEJ_GEOMETRY_FILE_FORMAT,
						   METAMORPH_GEOMETRY_FILE_FORMAT,
						   SVG_FILE_FORMAT};
enum IMAGEJ_REGION_TYPE {NONE_IMAGEJ_REGION_TYPE, RECTANGLE_IMAGEJ_REGION_TYPE, ELLIPSE_IMAGEJ_REGION_TYPE, POLYGON_IMAGEJ_REGION_TYPE};
enum CROSSING_DIRECTION {OUTIN_CROSSING, INOUT_CROSSING, ONOUT_CROSSING, ONIN_CROSSING};

struct myDynamicParam
{
	bool isACompartment = true;
	float D = -1;
	bool isTrapping_enable = false;
	bool areSites_abundant = false;
	float kt_on_abundant = -1;
	float kt_on_Nabundant = -1;
	float kt_off = -1;
	float site_density = -1;
	int nb_trappedPtcl = -1;
	bool isDDistributed = false;
	float Dt = -1;
	float poisson_mean = -1;
	float p_crossing_insideOut = 0.0f;
	float p_crossing_outsideIn = 0.0f;
};


class CELLENGINE_LIBRARYSHARED_EXPORT Region : public myGLObject
{
    friend void saveRegionsList(list<Region>& regions_l, std::string image_filePath, GEOMETRY_FILE_FORMAT format);
    friend bool getIntersectionRegion(Region& rgn1, Region& rgn2, vector<glm::vec2>& vertices);

public :

    Region(myMultiVector<glm::vec2>& r_mv, gstd::gMultiVector<glm::vec2>& r_gmv);
    Region(myMultiVector<glm::vec2>& r_mv, gstd::gMultiVector<glm::vec2>& r_gmv,
			 std::vector<glm::vec2>& r);
    ~Region();

    bool operator==(Region& rgn); //test if the region has the same vertices

	void render(myGLObject* renderingTarget);

	void setName(const string& name);
	string getName() const;

	void addPoint(glm::vec2& r);
	void addPoints(std::vector<glm::vec2>& r_v);
	const glm::vec2 getPoint(const int pt_idx);
    void clearRegion();


	glm::vec4 getColor() const;
    glm::vec4 getHighlightedColor() const;
    bool isHighlighted() const;
	void setColor(const glm::vec4& color);
    void setHighlightedColor(const glm::vec4& color);
    void setIsHighlighted(bool isHighlighted);


	int getSize() const;
	bool getRegionSubData(std::vector<glm::vec2> *region_points,const int offset,const int n_sub_data) const;
	bool isInside(const glm::vec2& r) const;
	float getMaximumRadiusSquared(const glm::vec2& r, bool& isInsideRgn) const;

    void addDynamicParam(ChemicalSpecies* spc);
    void removeDynamicParam(ChemicalSpecies* spc);

    void setIsACompartment(ChemicalSpecies* spc, bool new_state);
    void setIsDDistrubuted(ChemicalSpecies* spc, bool isDistributed);
    void setIsTrappingEnable(ChemicalSpecies* spc, bool isTrapping_enable);
    void setAreSitesAbundant(ChemicalSpecies* spc, bool isTrapping_enable);
    void setD(ChemicalSpecies* spc, float D);
    void setDtrapped(ChemicalSpecies* spc, float Dtrapped);
    void setKonAbundant(ChemicalSpecies* spc, float Kon_abundant);
    void setKonNAbundant(ChemicalSpecies* spc, float Kon_Nabundant);
    void setSiteDensity(ChemicalSpecies* spc, float site_density);
    void addTrappedPrtlToNb(ChemicalSpecies* spc);
    void setNumberTrappedPrtl(ChemicalSpecies* spc, int nb_prtl);
    void deleteATrappedPrtlToNb(ChemicalSpecies *spc);
    void setKoff(ChemicalSpecies* spc, float Koff);
    void setPoissonMean(ChemicalSpecies* spc, float poisson_mean);
    void setCrossing(ChemicalSpecies* spc, float p_crossing_insideOut, float p_crossing_outsideIn);

    bool isACompartment(ChemicalSpecies* spc) const;
    bool isDDistributed(ChemicalSpecies* spc) const;
    bool isTrappingActivated(ChemicalSpecies* specie) const;
    bool areSitesAbundant(ChemicalSpecies* spc) const;
    float getD(ChemicalSpecies* specie) const;
    float getDtrapped(ChemicalSpecies* spc) const;
    float getKonAbundant(ChemicalSpecies* spc) const;
    float getKonNAbundant(ChemicalSpecies* spc) const;
    float getSiteDensity(ChemicalSpecies* spc) const;
    float getCurrentSiteDensity(ChemicalSpecies* spc) const;
    int getNumberTrappedPrtl(ChemicalSpecies* spc) const;
    float getKoff(ChemicalSpecies* spc) const;
    float getPoissonMean(ChemicalSpecies* spc) const;
    float getCrossing(ChemicalSpecies* spc, CROSSING_DIRECTION cross_dir) const;


    void computeBarycenter();
    void computeRadiusSquared();
    void computeSurface();


	glm::vec2 getBarycenter() const;
	float getRadiusSquared() const;
	float getSurface() const;
	bool getBottomLeft(glm::vec2 &botoom_left) const;
	bool getTopRight(glm::vec2 &top_right) const;

//	void reflect(glm::vec2 r_start, glm::vec2 dr_start, int edge_to_avoid_start, glm::vec2 &r_end, glm::vec2 &dr_end, int &edge_to_avoid_end);
	void reflect(glm::vec2 r_start, glm::vec2 dr_start, int edge_to_avoid_start,
				 glm::vec2 &r_end, glm::vec2 &dr_end, int &edge_to_avoid_end,
				 RandomNumberGenerator& randomFactory,float p_crossing = 0.0f);

    void scaleRegion(float scaling_param);

	bool intersect(glm::vec2 r, glm::vec2 dr, int edge_to_avoid,
				   float& t_end, int& edge_idx, CROSSING_DIRECTION& cross_dir);

private :

	string m_name;
	glm::vec4 m_color;
    glm::vec4 m_highlighted_color;
    bool m_isHighlighted;

	mySubVector<glm::vec2> m_r;
	gstd::gSubVector<glm::vec2> m_r_gv;

    std::map<ChemicalSpecies*, myDynamicParam> m_dynamicParams_map;

	glm::vec2 m_barycenter_r;
    float m_region_radiusSquared;
    float m_surface;
};


uint jShortTocShort(char* jShort);

CELLENGINE_LIBRARYSHARED_EXPORT std::vector<std::vector<glm::vec2> > getPointsFromMetaMorphRgnFile(std::string metamorphRgn_file);
CELLENGINE_LIBRARYSHARED_EXPORT std::vector<glm::vec2> getPointsFromImageJRgnFile(std::string imageJRgn_file);
CELLENGINE_LIBRARYSHARED_EXPORT void saveRegionsList(std::list<Region>& regions_l,
													   std::string image_filePath,
													   GEOMETRY_FILE_FORMAT format);

bool intersect(glm::vec2 r, glm::vec2 dr, int edge_to_avoid,
			   float& t_end, int& edge_idx, CROSSING_DIRECTION& cross_dir);

CELLENGINE_LIBRARYSHARED_EXPORT bool getIntersectionRegion(Region& rgn1, Region& rgn2, vector<glm::vec2>& vertices);
CELLENGINE_LIBRARYSHARED_EXPORT float computeSurface(vector<glm::vec2>& r_v);


#endif // REGION_GPU_H
