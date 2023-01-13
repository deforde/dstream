#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"

#include "util.h"

#define GDB_EXTS_FILEPATH "/tmp/gdb_extensions.py"
#define GDB_CMDS_FILEPATH "/tmp/gdb_cmds"
// clang-format off
#define GDB_EXTS_CONTENT \
"from enum import Enum\n" \
"import argparse\n" \
"import gdb\n" \
"import json\n" \
"import socket\n" \
"import struct\n" \
"SOCK_PATH = \"/tmp/dstream_socket\"\n" \
"class Dstream_Data_Type(Enum):\n" \
"    U8 = 0\n" \
"    U16 = 1\n" \
"    U32 = 2\n" \
"    U64 = 3\n" \
"    I8 = 4\n" \
"    I16 = 5\n" \
"    I32 = 6\n" \
"    I64 = 7\n" \
"    F32 = 8\n" \
"    F64 = 9\n" \
"def getDataSize(d_ty: Dstream_Data_Type):\n" \
"    if d_ty == Dstream_Data_Type.U8 or d_ty == Dstream_Data_Type.I8:\n" \
"        return 1\n" \
"    elif d_ty == Dstream_Data_Type.U16 or d_ty == Dstream_Data_Type.I16:\n" \
"        return 2\n" \
"    elif (\n" \
"        d_ty == Dstream_Data_Type.U32\n" \
"        or d_ty == Dstream_Data_Type.I32\n" \
"        or d_ty == Dstream_Data_Type.F32\n" \
"    ):\n" \
"        return 4\n" \
"    elif (\n" \
"        d_ty == Dstream_Data_Type.U64\n" \
"        or d_ty == Dstream_Data_Type.I64\n" \
"        or d_ty == Dstream_Data_Type.F64\n" \
"    ):\n" \
"        return 8\n" \
"def getDataPackFmt(d_ty: Dstream_Data_Type):\n" \
"    if d_ty == Dstream_Data_Type.U8:\n" \
"        return \"B\"\n" \
"    elif d_ty == Dstream_Data_Type.U16:\n" \
"        return \"H\"\n" \
"    elif d_ty == Dstream_Data_Type.U32:\n" \
"        return \"I\"\n" \
"    elif d_ty == Dstream_Data_Type.U64:\n" \
"        return \"Q\"\n" \
"    elif d_ty == Dstream_Data_Type.I8:\n" \
"        return \"b\"\n" \
"    elif d_ty == Dstream_Data_Type.I16:\n" \
"        return \"h\"\n" \
"    elif d_ty == Dstream_Data_Type.I32:\n" \
"        return \"i\"\n" \
"    elif d_ty == Dstream_Data_Type.I64:\n" \
"        return \"q\"\n" \
"    elif d_ty == Dstream_Data_Type.F32:\n" \
"        return \"f\"\n" \
"    elif d_ty == Dstream_Data_Type.F64:\n" \
"        return \"d\"\n" \
"def packPacket(d_ty: Dstream_Data_Type, nm: str, data: list):\n" \
"    fmt = getDataPackFmt(d_ty)\n" \
"    dsz = getDataSize(d_ty)\n" \
"    padding = 0\n" \
"    nmsz = len(nm) + 1\n" \
"    if nmsz % dsz != 0:\n" \
"        padding = dsz - (nmsz % dsz)\n" \
"        nmsz += padding\n" \
"    psz = nmsz + len(data) * dsz\n" \
"    packet = [struct.pack(\"=iiQ\", d_ty.value, 0, psz)]\n" \
"    packet.append(bytes(nm, encoding=\"utf-8\"))\n" \
"    packet.append(bytes(\"\\0\" * (padding + 1), encoding=\"utf-8\"))\n" \
"    for x in data:\n" \
"        packet.append(struct.pack(f\"={fmt}\", x))\n" \
"    return packet\n" \
"class TraceDataCommand(gdb.Command):\n" \
"    def __init__(self):\n" \
"        super(TraceDataCommand, self).__init__(\"trace_data\", gdb.COMMAND_USER)\n" \
"        self.parser = argparse.ArgumentParser()\n" \
"        self.parser.add_argument(\"desc\", type=str, help=\"print json descriptor\")\n" \
"    def complete(self, text, word):\n" \
"        return gdb.COMPLETE_SYMBOL\n" \
"    def _decay_type(self, val):\n" \
"        while val.type.strip_typedefs().code == gdb.TYPE_CODE_PTR:\n" \
"            val = val.dereference()\n" \
"        return val\n" \
"    def invoke(self, args, from_tty):\n" \
"        desc = json.loads(args)\n" \
"        ident = desc[\"id\"]\n" \
"        packets = []\n" \
"        if \"@\" in ident:\n" \
"            ident, arr_len = ident.split(\"@\")\n" \
"            val = gdb.parse_and_eval(ident)\n" \
"            arr_len = gdb.parse_and_eval(arr_len)\n" \
"            arr_len = int(arr_len)\n" \
"            # assert val.type.strip_typedefs().code == gdb.TYPE_CODE_PTR\n" \
"            val = val.cast(\n" \
"                gdb.parse_and_eval(\n" \
"                    f\"*({val.type.strip_typedefs().target().name}(*)[{arr_len}])&{ident}\"\n" \
"                ).type.strip_typedefs()\n" \
"            ).dereference()\n" \
"            data = [float(x) for x in str(val).strip(\"{} \").split(\",\")]\n" \
"            packets.append(packPacket(Dstream_Data_Type.F32, ident, data))\n" \
"        else:\n" \
"            val = gdb.parse_and_eval(ident)\n" \
"            val = self._decay_type(val)\n" \
"            if val.type.strip_typedefs().code == gdb.TYPE_CODE_STRUCT:\n" \
"                for field in val.type.strip_typedefs().fields():\n" \
"                    field_val = self._decay_type(val[field.name])\n" \
"                    packets.append(\n" \
"                        packPacket(\n" \
"                            Dstream_Data_Type.F32,\n" \
"                            f\"{ident}.{field_name}\",\n" \
"                            float(str(field_val)),\n" \
"                        )\n" \
"                    )\n" \
"            else:\n" \
"                packets.append(\n" \
"                    packPacket(Dstream_Data_Type.F32, f\"{ident}\", [float(str(val))])\n" \
"                )\n" \
"        with socket.socket(family=socket.AF_UNIX, type=socket.SOCK_STREAM) as sock:\n" \
"            sock.connect(SOCK_PATH)\n" \
"            for packet in packets:\n" \
"               sock.sendall(b\"\".join(packet))\n" \
"TraceDataCommand()\n"
// clang-format on

static void buildStringList(cJSON *obj, const char *key, char ***pout) {
    cJSON *list = cJSON_GetObjectItemCaseSensitive(obj, key);
    if (!cJSON_IsArray(list)) {
        printf("config error for key: \"%s\"\n", key);
        exit(1);
    }
    cJSON *item;
    size_t i = 0;
    cJSON_ArrayForEach(item, list) { i++; }
    *pout = calloc(1, (i + 1) * sizeof(**pout));
    i = 0;
    cJSON_ArrayForEach(item, list) {
        if (!cJSON_IsString(item) || item->valuestring == NULL) {
            printf("config error for key: \"%s\", at index: %zu\n", key, i);
            exit(1);
        }
        (*pout)[i] = strdup(item->valuestring);
        i++;
    }
}

config_t configParse(const char *cfg_content) {
    config_t cfg = {0};

    cJSON *cjson = cJSON_ParseWithLength(cfg_content, strlen(cfg_content));

    cJSON *app = cJSON_GetObjectItemCaseSensitive(cjson, "app");
    if (!cJSON_IsString(app) || app->valuestring == NULL) {
        puts("config error for key: \"app\"");
        exit(1);
    }
    cfg.app = strdup(app->valuestring);

    buildStringList(cjson, "args", &cfg.args);
    buildStringList(cjson, "globals", &cfg.globals);

    //=========================================================================

    cJSON *locals = cJSON_GetObjectItemCaseSensitive(cjson, "locals");
    if (!cJSON_IsArray(locals)) {
        puts("config error for key: \"locals\"");
        exit(1);
    }
    cJSON *local;
    size_t i = 0;
    cJSON_ArrayForEach(local, locals) { i++; }
    cfg.locals = calloc(1, (i + 1) * sizeof(*cfg.locals));
    i = 0;
    cJSON_ArrayForEach(local, locals) {
        if (!cJSON_IsObject(local)) {
            printf("config error for key: \"locals\", at index: %zu\n", i);
            exit(1);
        }

        cfg.locals[i] = calloc(1, sizeof(**cfg.locals));

        buildStringList(local, "ids", &cfg.locals[i]->ids);

        cJSON *loc = cJSON_GetObjectItemCaseSensitive(local, "loc");
        if (!cJSON_IsString(loc) || loc->valuestring == NULL) {
            puts("config error for key: \"loc\"");
            exit(1);
        }
        cfg.locals[i]->loc = strdup(loc->valuestring);

        i++;
    }

    //=========================================================================

    cJSON *statics = cJSON_GetObjectItemCaseSensitive(cjson, "statics");
    if (!cJSON_IsArray(statics)) {
        puts("config error for key: \"statics\"");
        exit(1);
    }
    cJSON *sttc;
    i = 0;
    cJSON_ArrayForEach(sttc, statics) { i++; }
    cfg.statics = calloc(1, (i + 1) * sizeof(*cfg.statics));
    i = 0;
    cJSON_ArrayForEach(sttc, statics) {
        if (!cJSON_IsObject(sttc)) {
            printf("config error for key: \"statics\", at index: %zu\n", i);
            exit(1);
        }

        cfg.statics[i] = calloc(1, sizeof(**cfg.statics));

        buildStringList(sttc, "ids", &cfg.statics[i]->ids);

        cJSON *file = cJSON_GetObjectItemCaseSensitive(sttc, "file");
        if (!cJSON_IsString(file) || file->valuestring == NULL) {
            puts("config error for key: \"file\"");
            exit(1);
        }
        cfg.statics[i]->file = strdup(file->valuestring);

        i++;
    }

    //=========================================================================

    cJSON_Delete(cjson);

    return cfg;
}

void configDestroy(config_t cfg) {
    free(cfg.app);

    for (size_t i = 0; cfg.args[i]; i++) {
        free(cfg.args[i]);
    }
    free(cfg.args);

    for (size_t i = 0; cfg.globals[i]; i++) {
        free(cfg.globals[i]);
    }
    free(cfg.globals);

    for (size_t i = 0; cfg.locals[i]; i++) {
        for (size_t j = 0; cfg.locals[i]->ids[j]; j++) {
            free(cfg.locals[i]->ids[j]);
        }
        free(cfg.locals[i]->ids);
        free(cfg.locals[i]->loc);
        free(cfg.locals[i]);
    }
    free(cfg.locals);

    for (size_t i = 0; cfg.statics[i]; i++) {
        for (size_t j = 0; cfg.statics[i]->ids[j]; j++) {
            free(cfg.statics[i]->ids[j]);
        }
        free(cfg.statics[i]->ids);
        free(cfg.statics[i]->file);
        free(cfg.statics[i]);
    }
    free(cfg.statics);
}

void configGenGDBCommands(config_t cfg) {
    FILE *f = fopen(GDB_CMDS_FILEPATH, "w");
    if (f == NULL) {
        printf("fopen error: \"%s\"\n", GDB_CMDS_FILEPATH);
        exit(1);
    }

    fprintf(f,
            "source %s\n"
            "set breakpoint pending on\n"
            "set print elements unlimited\n"
            "set print repeats unlimited\n"
            "set pagination off\n",
            GDB_EXTS_FILEPATH);

    for (size_t i = 0; cfg.globals[i]; i++) {
        fprintf(f,
                "watch %s\n"
                "commands\n"
                "silent\n"
                "trace_data {\"id\": \"%s\"}\n"
                "c\n"
                "end\n",
                cfg.globals[i], cfg.globals[i]);
    }

    for (size_t i = 0; cfg.locals[i]; i++) {
        fprintf(f,
                "b %s\n"
                "commands\n"
                "silent\n",
                cfg.locals[i]->loc);
        for (size_t j = 0; cfg.locals[i]->ids[j]; j++) {
            fprintf(f, "trace_data {\"id\": \"%s\"}\n", cfg.locals[i]->ids[j]);
        }
        fprintf(f, "c\n"
                   "end\n");
    }

    for (size_t i = 0; cfg.statics[i]; i++) {
        for (size_t j = 0; cfg.statics[i]->ids[j]; j++) {
            fprintf(f,
                    "watch %s::%s\n"
                    "commands\n"
                    "silent\n"
                    "trace_data {\"id\": \"%s\"}\n"
                    "c\n"
                    "end\n",
                    cfg.statics[i]->file, cfg.statics[i]->ids[j],
                    cfg.statics[i]->ids[j]);
        }
    }

    fprintf(f, "r\n"
               "q\n");

    fclose(f);
}

void configGenGDBExtensions(void) {
    writeFile(GDB_EXTS_FILEPATH, GDB_EXTS_CONTENT, strlen(GDB_EXTS_CONTENT));
}
