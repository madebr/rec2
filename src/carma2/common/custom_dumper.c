#include "dumper.h"

#include "rec2_macros.h"

void dump_br_model(FILE *file, const br_model *s, unsigned int depth, unsigned int max_depth) {
    if (depth == max_depth) {
        return;
    }
    fprintf(file, "(br_model) {\n");
    fprintf(file, ".identifier = \"%s\"\n", s->identifier);
    fprintf(file, ".nvertices = %u\n", (unsigned)s->nvertices);
    fprintf(file, ".vertices = {\n");
    for (br_uint_16 i = 0; i < s->nvertices; i++) {
        fprintf(file, "[%u] = ", (unsigned)i);
        dump_br_vertex(file, &s->vertices[i], depth + 1, max_depth);
    }
    fprintf(file, ".nfaces = %u\n", (unsigned)s->nfaces);
    for (br_uint_16 i = 0; i < s->nfaces; i++) {
        fprintf(file, "[%u] = ", (unsigned)i);
        dump_br_face(file, &s->faces[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");
    fprintf(file, ".pivot = ");
    dump_br_vector3(file, &s->pivot, depth + 1, max_depth);
    fprintf(file, ".flags = 0x%02x\n", (unsigned)s->flags);
    fprintf(file, ".crease_angle = 0x%04x\n", (unsigned)s->crease_angle);
    fprintf(file, ".radius = %g\n", s->radius);
    fprintf(file, ".bound = ");
    dump_br_bounds3(file, &s->bounds, depth + 1, max_depth);
    fprintf(file, ".nprimitivie_list = %u\n", (unsigned)s->nprimitive_lists);

    fprintf(file, ".stored = ");
    if (V11MODEL(s) == NULL) {
        fprintf(file, "NULL\n");
    } else {
        dump_v11model(file, V11MODEL(s), depth + 1, max_depth);
    }

    fprintf(file, "}\n");

    fprintf(file, "}\n");

    fprintf(file, "}\n");
}

void dump_v11model(FILE *file, const v11model *s, unsigned int depth, unsigned int max_depth) {

    if (depth > max_depth) {
        return;
    }
    fprintf(file, "(v11model) {\n");
    fprintf(file, ".size = %u\n", s->size);
    fprintf(file, ".flags = 0x%08x\n", s->flags);
    dump_br_vector3(file, &s->pivot, depth + 1, max_depth);
    fprintf(file, ".ngroups = %u\n", s->ngroups);
    fprintf(file, ".groups = %u\n", s->ngroups);
    for (br_uint_16 i = 0; i < s->ngroups; i++) {
        fprintf(file, "[%u]", i);
        dump_v11group(file, &s->groups[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");
}
void dump_v11group(FILE* file, const v11group* s, unsigned int depth, unsigned int max_depth) {

    if (depth > max_depth) {
        return;
    }
    fprintf(file, "(v11group) {\n");

    fprintf(file, ".nedges = %u\n", s->nedges);
    fprintf(file, ".nvertices = %u\n", s->nvertices);

    fprintf(file, ".vertices = {\n");
    for (br_uint_16 i = 0; i < s->nvertices; i++) {
        fprintf(file, "[%u] = ", i);
        dump_fmt_vertex(file, &s->vertices[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");

    fprintf(file, ".vertex_colour = {\n");
    for (br_uint_16 i = 0; i < s->nvertices; i++) {
        fprintf(file, "[%u] = 0x%08x\n", i, s->vertex_colours[i]);
    }
    fprintf(file, "}\n");

    fprintf(file, ".nfaces = %u\n", s->nfaces);

    fprintf(file, ".faces = {\n");
    for (br_uint_16 i = 0; i < s->nfaces; i++) {
        dump_v11face(file, &s->faces[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");

    if (s->face_colours != NULL) {
        fprintf(file, ".face_colours = {\n");
        for (br_uint_16 i = 0; i < s->nfaces; i++) {
            fprintf(file, "[%u] = 0x%08x\n", i, s->face_colours[i]);
        }
        fprintf(file, "}\n");
    } else {
        fprintf(file, ".face_colours = NULL\n");
    }

    fprintf(file, ".face_user = {\n");
    for (br_uint_16 i = 0; i < s->nfaces; i++) {
        fprintf(file, "[%u] = 0x%04x\n", i, s->face_user[i]);
    }
    fprintf(file, "}\n");

    fprintf(file, "}\n");
}

void dump_tBrender_storage(FILE *file, const tBrender_storage *s, unsigned int depth, unsigned int max_depth) {
    if (depth > max_depth) {
        return;
    }
    fprintf(file, "(tBrender_storage) {\n");

    fprintf(file, ".max_models = %d\n", s->max_models);
    fprintf(file, ".models_count = %d\n", s->models_count);
    fprintf(file, ".models = {\n");
    for (int i = 0; i < s->models_count; i++) {
        fprintf(file, "[%d] = ", i);
        dump_br_model(file, s->models[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");

    fprintf(file, ".max_materials = %d\n", s->max_materials);
    fprintf(file, ".materials_count = %d\n", s->materials_count);
    fprintf(file, ".models = {\n");
    for (int i = 0; i < s->materials_count; i++) {
        fprintf(file, "[%d] = ", i);
        dump_br_material(file, s->materials[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");

    fprintf(file, ".max_shade_tables = %d\n", s->max_shade_tables);
    fprintf(file, ".shade_tables_count = %d\n", s->shade_tables_count);
    fprintf(file, ".shade_tables = {\n");
    for (int i = 0; i < s->shade_tables_count; i++) {
        fprintf(file, "[%d] = ", i);
        dump_br_pixelmap(file, s->shade_tables[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");

    fprintf(file, ".max_pixelmaps = %d\n", s->max_pixelmaps);
    fprintf(file, ".pixelmaps_count = %d\n", s->pixelmaps_count);
    fprintf(file, ".pixelmaps = {\n");
    for (int i = 0; i < s->pixelmaps_count; i++) {
        fprintf(file, "[%d] = ", i);
        dump_br_pixelmap(file, s->pixelmaps[i], depth + 1, max_depth);
    }
    fprintf(file, "}\n");

    fprintf(file, ".max_sounds = %d\n", s->max_sounds);
    fprintf(file, ".sounds_count = %d\n", s->sounds_count);
    fprintf(file, ".sounds = {\n");
    for (int i = 0; i < s->sounds_count; i++) {
        fprintf(file, "[%d] = %p\n", i, s->sounds[i]);
    }
    fprintf(file, "}\n");
}

void dump_tCollision_shape_polyhedron_data(FILE *file, const tCollision_shape_polyhedron_data *s, unsigned int depth, unsigned int max_depth) {
    if (depth > max_depth) {
        return;
    }
    fprintf(file, "(tCollision_shape_polyhedron_data) {\n");
    fprintf(file, ".count_points = %d\n", s->count_points);
    fprintf(file, ".count_edges = %d\n", s->count_edges);
    fprintf(file, ".count_planes = %d\n", s->count_planes);
    for (int i = 0; i < s->count_points; i++) {
        fprintf(file, ".points[%d] = { %g, %g, %g },\n", i, s->points[i].v[0], s->points[i].v[1], s->points[i].v[2]);
    }
    for (int i = 0; i < s->count_edges; i++) {
        fprintf(file, ".edges[%d] = { %u, %u },\n", i, s->edges[i].index1, s->edges[i].index2);
    }
    for (int i = 0; i < s->count_planes; i++) {
        fprintf(file, ".planes[%d] = { %g, %g, %g, %g },\n", i, s->planes[i].v[0], s->planes[i].v[1], s->planes[i].v[2], s->planes[i].v[3]);
    }
    fprintf(file, "}\n");
}
