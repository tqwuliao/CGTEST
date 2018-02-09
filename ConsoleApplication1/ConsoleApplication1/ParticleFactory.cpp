#include "ParticleFactory.h"
#include "CacheManager.h"

shader* ParticleFactory::_shader = 0;

std::vector<glm::vec3> drops(100);
int dropsize=0;

ParticleFactory & ParticleFactory::Instance()
{
	static ParticleFactory res;
	return res;
	// TODO: 在此处插入 return 语句
}

void ParticleFactory::draw(shader & s)
{
	for (std::vector<ParticleBundle>::iterator it = bundles.begin();it != bundles.end();)
	{
		//printf("[particle] %f,%f,%f\n", it->anchor.x, it->anchor.y, it->anchor.z);
		it->draw(s);
		if (it->end()) {
			it = bundles.erase(it);
			//
		} else it++;
	}
	
	//dropsize = 0;
}

ParticleFactory::ParticleFactory()
{
}


ParticleFactory::~ParticleFactory()
{
}

inline bool ParticleBundle::dropUpd(Particle & p) {
	if (p.lifespan-- == 0) {
		p.lifespan = 0;
		p.tone.a = 0;
		if (dropsize > 0) {
			p.position = drops[--dropsize];
			p.lifespan = 20;
			p.tone.a = 0.8;
			p.zoom = 0.5;
		}
		return true;
	}
	if (p.lifespan >= 0 && p.lifespan <= 20) {
		p.tone.a -= (p.tone.a - 1.0) / 10.0;
	}
	p.zoom += rand() % 10 / 5000.0;
	return true;
}

ParticleBundle::ParticleBundle(int num, bool _loop, ParticleUpdate _u, ParticleUpdate _g) :
	updateFunc(_u),
	generateFunc(_g),
	texture(NULL),
	width(6),
	height(128),
	loop(_loop),
	size(num),
	lifespan(0),
	visible(true)
{
	refresh();
	glCreateVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//printf((const char*)glewGetErrorString(glGetError()));
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * size, NULL, GL_DYNAMIC_DRAW);
	//printf((const char*)glewGetErrorString(glGetError()));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle,position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, tone));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, velocity));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(Particle), (GLvoid*)offsetof(Particle, lifespan));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, zoom));

	//printf((const char*)glewGetErrorString(glGetError()));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//printf((const char*)glewGetErrorString(glGetError()));
	glBindVertexArray(0);
}

void ParticleBundle::refresh() {
	//count = size;
	kids.resize(size);
	for (int i = 0;i < size;i++)
	{
		generateFunc(kids[i]);
	}
}

void ParticleBundle::update() {
	count = size;
	for (std::vector<Particle>::iterator i = kids.begin();i != kids.end();i++)
	{
		//if (!updateFunc(*i))	i = kids.erase(i);
		if(!updateFunc(*i)) count--;
		
	}
	if (count <= 0 && loop) refresh();
}

void ParticleBundle::draw(shader & _s) {
	if (!visible) return;
	update();
	if (end()) return;
	//ParticleFactory::_shader->run();
	_s.bind("anchor", this->anchor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->texture->id());
	glActiveTexture(GL_TEXTURE0);
	_s.bind("tex", 1);
	_s.bind("width", width);
	_s.bind("height", height);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle)*kids.size(), &kids[0]);
	glDrawArrays(GL_POINTS, 0, kids.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
}

ParticleUpdate testfunc;

void addExplosion(glm::vec3 pos) {
	if (ParticleFactory::Instance().bundles.size() < 1) {
		ParticleFactory::Instance().bundles.emplace_back(1, false);
		auto& it = ParticleFactory::Instance().bundles.back();
		it.visible = false;
		it.updateFunc = it.cacheUpd;
		it.texture=(
			CacheManager<Texture>::Instance().setOrGetResource("./resources/spot.png"));
		
		ParticleFactory::Instance().bundles.emplace_back(2000, false, ParticleBundle::dropUpd, ParticleBundle::dropGen);
		auto& itt = ParticleFactory::Instance().bundles.back();
		//itt.visible = false;
		itt.width = 128;
		itt.height = 64;
		itt.texture = (
			CacheManager<Texture>::Instance().setOrGetResource("./resources/drop.jpg"));
		
	}
	ParticleFactory::Instance().bundles.emplace_back(500, true);
	auto& it = ParticleFactory::Instance().bundles.back();

	it.anchor = { 0,20,0 };
	
	it.texture=(
		CacheManager<Texture>::Instance().setOrGetResource("./resources/spot.png"));
	
	it.updateFunc = testfunc;


}
