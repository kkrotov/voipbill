#pragma once

#include "BaseTest.h"

/**
 * Класс проверяющий корректную работу функции removeHtmlTags.
 *
 * Проверка делается с помощью сравнения реальных результатов
 *  работы функции и ожиданных.
 *
 */
class RemoveHtmlTagsTest : public BaseTest {

    RemoveHtmlTagsTest (RemoveHtmlTagsTest&);
    RemoveHtmlTagsTest& operator= (RemoveHtmlTagsTest&);

    string resultStr;
    bool   resultReturn;

    const static unsigned int maxIndex = 3;

public:

    RemoveHtmlTagsTest ();

    virtual bool setResult   (unsigned int index) override;
    virtual bool checkResult (unsigned int index) override;

    virtual unsigned int getMaxIndex () const override;
    virtual const char* getTestName  () const override;

};
