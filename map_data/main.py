import shapefile
import matplotlib.pyplot as plt
import numpy as np


def load_polygons(shp):
    data = []
    for s in shp.shapes():
        a = []
        b = []
        start = (0, 0)
        flag = True
        for p in s.points:
            a.append([p[0], p[1]])
            if flag:
                flag = False
                start = p
            elif p == start:
                flag = True
                b.append(a)
                a = []
        data.append(b)
    return data


def load_points(shp):
    data = []
    for s in shp.shapes():
        a = []
        for p in s.points:
            a.append([p[0], p[1]])
        data.append(a[0])
    return data


def load_names(shp, label):
    data = []
    index = 0
    i = 0
    for f in shp.fields:
        if f[0] == label:
            index = i
        i += 1
    for r in shp.records():
        if type(r[index]) is str:
            data.append(r[index].rstrip('\x00'))
        else:
            data.append(r[index])
    return data


def load_fields(shp):
    data = []
    for f in shp.fields:
        data.append(f[0])
    return data


def plot_polygons(data, mode='-'):
    for i in range(len(data)):
        for p in data[i]:
            b = np.array(p).T
            plt.plot(b[0], b[1], mode)


def plot_points(data, mode='-'):
    for i in range(len(data)):
        p = data[i]
        plt.plot(p[0], p[1], mode)


countries_shp = shapefile.Reader("data/countries/ne_50m_admin_0_countries.shp")
states_shp = shapefile.Reader("data/states/ne_10m_admin_1_states_provinces.shp")
cities_shp = shapefile.Reader("data/cities/ne_10m_populated_places.shp")

countries_geometry = load_polygons(countries_shp)
states_geometry = load_polygons(states_shp)
cities_geometry = load_points(cities_shp)


countries_name = load_names(countries_shp, 'NAME')
countries_continent = load_names(countries_shp, 'CONTINENT')
countries_admin = load_names(countries_shp, 'ADMIN')

states_name = load_names(states_shp, 'hasc_maybe')
states_name2 = load_names(states_shp, 'woe_id')
for i in range(len(states_name)):
    if states_name[i] == '':
        states_name[i] = states_name2[i].split(",")[0]

states_local_name = load_names(states_shp, 'woe_label')
states_country = load_names(states_shp, 'admin')
states_admin = load_names(states_shp, 'sov_a3')

cities_name = load_names(cities_shp, 'NAMEALT')
cities_state = load_names(cities_shp, 'ADM0_A3')
cities_country = load_names(cities_shp, 'SOV_A3')
cities_admin = load_names(cities_shp, 'ADM0NAME')

def plot():
    plot_points(cities_geometry, 'g.')
    plot_polygons(states_geometry, 'b-')
    plot_polygons(countries_geometry, 'r-')
    plt.show()


def info(shp, index=0):
    r = shp.records()
    for i in range(len(shp.fields)):
        f = shp.fields[i]
        if len(r[index]) > i:
            v = r[index][i]
            if type(v) is str:
                print(f[0], ":", v.rstrip('\x00'))
            else:
                print(f[0], ":", v)


def test():
    index = 0
    for i in range(len(countries_name)):
        if str(countries_name[i]) == 'Germany':
            index = i

    cname = countries_name[index]
    cadmin = countries_admin[index]

    print(index, "country", cname, "(", cadmin, ") in", countries_continent[index])

    for i in range(len(states_name)):
        sname = states_name[i]
        slocal = states_local_name[i]
        scountry = states_country[i]
        sadmin = states_admin[i]
        if scountry == cname:
            print(" ", i, "state", sname, "(", slocal, sadmin, ")", "in", scountry)
            for j in range(len(cities_name)):
                name = cities_name[j]
                country = cities_country[j]
                state = cities_state[j]
                admin = cities_admin[j]
                if country == cname:
                    if state == slocal or state == sname:
                        print("   ", j, "city", name, "(", admin, ")", "in", state)


def write(file, type, name, admin, geom, point, divisor=1, center=False):
    file.write(type)
    file.write(":")
    file.write(name)
    file.write(":")
    file.write(admin)
    file.write(":")
    if point:
        file.write(str(geom[0]))
        file.write(",")
        file.write(str(geom[1]))
        file.write(";")
    else:
        if not center:
            for polygon in geom:
                i = 0
                for vertex in polygon:
                    if i % divisor == 0 or i == len(polygon) - 1:
                        file.write(str(vertex[0]))
                        file.write(",")
                        file.write(str(vertex[1]))
                        file.write(";")
                    i += 1
                file.write(":")
        else:
            count = 0
            sumx = 0
            sumy = 0
            for polygon in geom:
                for vertex in polygon:
                    sumx += vertex[0]
                    sumy += vertex[1]
                    count += 1
            sumx /= count
            sumy /= count
            file.write(str(sumx))
            file.write(",")
            file.write(str(sumy))
            file.write(";")
    file.write("\n")


def generate():
    states_list = [0 for x in states_name]
    cities_list = [0 for x in cities_name]
    file = open("worldmap.txt", mode="w", encoding="utf-8")
    for index in range(len(countries_name)):
        cname = countries_name[index]
        cadmin = countries_admin[index]
        ccontinent = countries_continent[index]
        write(file, 'country', cname, ccontinent, countries_geometry[index], False, 1)
        for i in range(len(states_name)):
            sname = states_name[i]
            slocal = states_local_name[i]
            scountry = states_country[i]
            sadmin = states_admin[i]
            if scountry == cname and states_list[i] == 0:
                write(file, 'state', sname, scountry, states_geometry[i], False, 10)
                states_list[i] = 1
                for j in range(len(cities_name)):
                    name = cities_name[j]
                    country = cities_country[j]
                    state = cities_state[j]
                    admin = cities_admin[j]
                    if country == cname and cities_list[j] == 0:
                        if state == slocal or state == sname:
                            write(file, 'city', name, state, cities_geometry[i], True)
                            cities_list[j] = 1


generate()
