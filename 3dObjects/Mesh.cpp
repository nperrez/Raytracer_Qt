//
// Created by nicolas on 09.04.26.
//

#include "Mesh.h"
#include <cstdio>
#include <fstream>
#include <limits>

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Triangle> tris) : triangles(std::move(tris)) {}

// ---------------------------------------------------------------------------
// .mtl parser
// ---------------------------------------------------------------------------

std::unordered_map<std::string, Material> Mesh::parseMtl(const std::string &mtlPath) {
    std::unordered_map<std::string, Material> materials;
    std::ifstream file(mtlPath);
    if (!file.is_open()) return materials;

    // Per-material state, reset on each newmtl
    std::string name;
    Color Ka(0.2, 0.2, 0.2), Kd(0.8, 0.8, 0.8), Ks(0, 0, 0), Tf(1, 1, 1);
    double Ns = 0.0, Ni = 1.0, d = 1.0, roughness = 0.0;
    int illum = 1;
    bool hasMaterial = false;

    auto saveCurrent = [&]() {
        if (hasMaterial)
            materials.emplace(name, Material(Ka, Kd, Ks, Tf, Ns, Ni, d, roughness, illum));
    };

    auto resetState = [&]() {
        Ka = Color(0.2, 0.2, 0.2); Kd = Color(0.8, 0.8, 0.8);
        Ks = Color(0, 0, 0);       Tf = Color(1, 1, 1);
        Ns = 0.0; Ni = 1.0; d = 1.0; roughness = 0.0; illum = 1;
    };

    std::string line;
    while (std::getline(file, line)) {
        // Strip Windows CR
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;

        const char *p = line.c_str();
        float fr, fg, fb;

        if (line.compare(0, 7, "newmtl ") == 0) {
            saveCurrent();
            name = line.substr(7);
            while (!name.empty() && (name.back() == ' ')) name.pop_back();
            resetState();
            hasMaterial = true;
        } else if (p[0] == 'K' && p[2] == ' ') {
            sscanf(p + 3, "%f %f %f", &fr, &fg, &fb);
            if      (p[1] == 'a') Ka = Color(fr, fg, fb);
            else if (p[1] == 'd') Kd = Color(fr, fg, fb);
            else if (p[1] == 's') Ks = Color(fr, fg, fb);
            // Ke (emissive) not rendered — skip
        } else if (p[0] == 'T' && p[1] == 'f' && p[2] == ' ') {
            sscanf(p + 3, "%f %f %f", &fr, &fg, &fb);
            Tf = Color(fr, fg, fb);
        } else if (p[0] == 'N' && p[1] == 's' && p[2] == ' ') {
            sscanf(p + 3, "%lf", &Ns);
        } else if (p[0] == 'N' && p[1] == 'i' && p[2] == ' ') {
            sscanf(p + 3, "%lf", &Ni);
        } else if (p[0] == 'd' && (p[1] == ' ' || p[1] == '\t')) {
            sscanf(p + 2, "%lf", &d);
        } else if (line.compare(0, 3, "Tr ") == 0) {
            double tr; sscanf(p + 3, "%lf", &tr);
            d = 1.0 - tr;
        } else if (line.compare(0, 6, "illum ") == 0) {
            sscanf(p + 6, "%d", &illum);
        }
        // map_*, sharpness, refl, etc. — not supported, skip
    }
    saveCurrent();
    return materials;
}

// ---------------------------------------------------------------------------
// .obj parser
// ---------------------------------------------------------------------------

Mesh Mesh::fromObj(const std::string &objPath, double scale, bool ownMaterial, const Material& material) {
    // Directory prefix for resolving relative mtllib paths
    std::string objDir;
    const size_t slash = objPath.find_last_of("/\\");
    if (slash != std::string::npos) objDir = objPath.substr(0, slash + 1);

    std::unordered_map<std::string, Material> materials;
    Material currentMaterial = material;   // default MESH material until first usemtl

    std::vector<Vector3d> vertices;
    std::vector<Triangle> triangles;
    vertices.reserve(1024);
    triangles.reserve(2048);

    std::ifstream file(objPath);
    if (!file.is_open()) return Mesh();

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;

        const char *p = line.c_str();
        float fx, fy, fz;

        if (p[0] == 'v' && p[1] == ' ') {
            // Vertex position
            sscanf(p + 2, "%f %f %f", &fx, &fy, &fz);
            vertices.emplace_back(fx * scale, fy * scale, fz * scale);

        } else if (p[0] == 'f' && p[1] == ' ') {
            // Face — supports v, v/t, v/t/n, v//n; quads/ngons fan-triangulated
            int faceVerts[32];   // 32 verts per face is more than enough
            int faceCount = 0;
            const char *s = p + 2;
            while (*s && faceCount < 32) {
                while (*s == ' ') s++;
                if (!*s) break;
                int idx;
                if (sscanf(s, "%d", &idx) != 1) break;
                // Negative indices count from end of current vertex list
                if (idx < 0) idx = (int)vertices.size() + idx + 1;
                faceVerts[faceCount++] = idx - 1;   // convert to 0-based
                // Skip past this token (v, v/t, v/t/n, v//n)
                while (*s && *s != ' ') s++;
            }
            // Fan triangulation: (0,1,2), (0,2,3), ...
            for (int i = 1; i + 1 < faceCount; i++) {
                const int ai = faceVerts[0], bi = faceVerts[i], ci = faceVerts[i + 1];
                if (ai < (int)vertices.size() && bi < (int)vertices.size() && ci < (int)vertices.size())
                    triangles.emplace_back(vertices[ai], vertices[bi], vertices[ci], currentMaterial);
            }

        } else if (line.compare(0, 7, "usemtl ") == 0 && !ownMaterial) {
            std::string mtlName = line.substr(7);
            while (!mtlName.empty() && mtlName.back() == ' ') mtlName.pop_back();
            auto it = materials.find(mtlName);
            if (it != materials.end()) currentMaterial = it->second;

        } else if (line.compare(0, 7, "mtllib ") == 0 && !ownMaterial) {
            std::string mtlFile = line.substr(7);
            while (!mtlFile.empty() && mtlFile.back() == ' ') mtlFile.pop_back();
            materials = parseMtl(objDir + mtlFile);
        }
        // vn, vt, g, o, s — ignored
    }

    return Mesh(std::move(triangles));
}

// ---------------------------------------------------------------------------
// Intersection — walks all triangles, returns closest hit
// ---------------------------------------------------------------------------

Hit Mesh::intersect(Ray ray) const {
    Hit closest(-1, Vector3d(0, 0, 0), Vector3d(0, 0, 0), Material(Color(0, 0, 0)));
    double minLambda = std::numeric_limits<double>::infinity();
    for (const Triangle &tri : triangles) {
        Hit h = tri.intersect(ray);
        if (h.getLambda() > 0 && h.getLambda() < minLambda) {
            minLambda = h.getLambda();
            closest = h;
        }
    }
    return closest;
}
