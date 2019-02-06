////////////////////////////////////////////////////////////////////////////////
// FormulaParse.cpp
#include "stdafx.h"
#include "FormulaParse.h"
#include "TLog.h"

CFormulaParse g_FormulaParse;

CFormulaParse::CFormulaParse()
{
	this->m_bFileLoad = false;
}

CFormulaParse::~CFormulaParse()
{
}

void CFormulaParse::LoadFile(char * szFile)
{
	this->m_bFileLoad = false;
	this->m_mapExcOptFormulaData.clear();
	this->m_mapExpFormulaData.clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if (res.status != pugi::status_ok)
	{
		g_Log.MsgBox("%s file load fail (%s)", szFile, res.description());
		return;
	}

	pugi::xml_node main = file.child("FormulaData");
	pugi::xml_node exp_data = main.child("Exp");
	pugi::xml_node excopt_data = main.child("ExcellentOption");

	for (pugi::xml_node formula = exp_data.child("Formula"); formula; formula = formula.next_sibling())
	{
		int iID = formula.attribute("ID").as_int();
		std::string szFormula = formula.attribute("Data").as_string();

		this->m_mapExpFormulaData.insert(std::pair<int, std::string>(iID, szFormula));
	}

	for (pugi::xml_node formula = excopt_data.child("Formula"); formula; formula = formula.next_sibling())
	{
		int iID = formula.attribute("ID").as_int();
		std::string szFormula = formula.attribute("Data").as_string();

		this->m_mapExcOptFormulaData.insert(std::pair<int, std::string>(iID, szFormula));
	}

	this->m_bFileLoad = true;
}

std::string * CFormulaParse::GetFormula(int iFormulaType, int iFormulaID)
{
	if (iFormulaType < FORMULA_TYPE_EXP || iFormulaType > FORMULA_TYPE_UNK)
	{
		return nullptr;
	}

	std::map<int, std::string>::iterator It;

	switch (iFormulaType)
	{
		case FORMULA_TYPE_EXP:
			{
				It = this->m_mapExpFormulaData.find(iFormulaID);

				if (It == this->m_mapExpFormulaData.end())
				{
					return nullptr;
				}
			}
			break;
		case FORMULA_TYPE_EXCELLENT:
		{
			It = this->m_mapExcOptFormulaData.find(iFormulaID);

			if (It == this->m_mapExcOptFormulaData.end())
			{
				return nullptr;
			}
		}
		break;
		default:
			return nullptr;
	}

	return &It->second;
}

double CFormulaParse::UseFormula(int iFormulaType, int iFormulaID, char * ArgNames, int iArgCount, ...)
{
	std::string * szFormula = this->GetFormula(iFormulaType, iFormulaID);

	if (szFormula == nullptr)
	{
		return -1.0;
	}

	va_list vList;
	std::vector<double> args;

	va_start(vList, iArgCount);
	args.clear();

	for (int i = 0; i < iArgCount; i++)
	{
		args.push_back(va_arg(vList, double));
	}

	mu::Parser p;

	for (int i = 0; i < iArgCount; i++)
	{
		std::string szArgName(1, ArgNames[i]);
		p.DefineVar(szArgName, &args[i]);
	}

	p.SetExpr(*szFormula);
	return p.Eval();
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

