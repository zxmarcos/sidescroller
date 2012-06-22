#ifndef XYOBJECT_H
#define XYOBJECT_H


// Classe abstrata para qualquer objeto que esteja relacionado
// com o "mundo", contendo as coordenadas e a velocidade dos eixos
class XYObject
{
	public:
		XYObject();
		~XYObject();

		float x() const;
		float y() const;
		float lastx() const;
		float lasty() const;
		virtual int width() const;
		virtual int height() const;
		float velx() const;
		float vely() const;

		void setx(float x);
		void sety(float y);
		void setvelx(float x);
		void setvely(float y);
	protected:
		float xvel;
		float yvel;
		float xpos;
		float ypos;
		float xlast;
		float ylast;
		float xvellast;
		float yvellast;
		int xsize;
		int ysize;
};


#endif // XYOBJECT_H
