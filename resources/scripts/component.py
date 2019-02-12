

def create_component(name, data, dependencies):
    template = """
class {name}
{{
    {dependencies}
    {data}
    
    {methods}
}};
    """
    a = '\n\t'.join( ['{} {};'.format(type, name) for type, name in data] )
    b = '\n\t'.join( ['{}& {};'.format(type, name) for type, name in dependencies] )
    c = 'std::vector<unsigned> Dependencies() {{ return {{{types}}} }}'.format(types='\n'.join('ClassID<{}>()'.format(type) for type, name in dependencies))

    return template.format(name=name, dependencies=a, data=b, methods=c)



print(create_component(
    name='Transform',
    data=[('glm::vec3', 'position'), ('glm::vec3', 'rotation'), ('glm::vec3', 'scale')],
    dependencies=[]
))
print(create_component(
    name='MeshRenderer',
    data=[('GLuint', 'texture'), ('GLuint', 'mesh')],
    dependencies=[('Transform', 'transform')]
))
print(create_component(
    name='RigidBody',
    data=[('float', 'mass'), ('glm::vec3', 'velocity')],
    dependencies=[('Transform', 'transform')]
))