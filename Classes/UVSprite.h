#ifndef __UVSprite_H__
#define __UVSprite_H__

#include "cocos2d.h"
USING_NS_CC;


class UVSprite : public cocos2d::Sprite
{
public:
	static UVSprite* create(const std::string& filename);
	UVSprite();
	virtual ~UVSprite();
	//重写draw
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
	void onDraw(const Mat4 &transform, uint32_t flags);
	//设置并启用uv纹理(若不调用,与普通Sprite行为一致)
	bool setUVTexture(const std::string& filename);
protected:
	void __setUVTexture(cocos2d::Texture2D *texture);
	cocos2d::Texture2D*       _uvTexture;
	CustomCommand _customCommand;
	Vec3 _verticesTransformed[4];

	//uvTexture与Sprite自身纹理的宽高比例,uv纹理是否拉伸
	Vec2 _uvScale;
	GLuint _uvScaleLoc;
	CC_SYNTHESIZE(bool, _uvStretch, UvStretch);

	//uv纹理是否受原图alpha值影响
	CC_SYNTHESIZE(bool, _uvAlphaFilter, UvAlphaFilter);
	GLuint _uvAlphaFilterLoc;

	//uv纹理透明度
	CC_SYNTHESIZE(float, _uvOpacity, UvOpacity);
	GLuint _uvOpacityLoc;

	//uv纹理移动速度,坐标偏移量
	CC_SYNTHESIZE(Vec2, _uvVelocity, UvVelocity);
	CC_SYNTHESIZE_READONLY(Vec2, _uvPosition, UvPosition);
	GLuint _uvVelocityLoc;
};

#endif // __UVSprite_H__
